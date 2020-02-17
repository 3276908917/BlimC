#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// This is only a diagnostic function, to ensure
	// that the binding was performed properly.
int add(int i, int j) {
    return i + j;
}

/*c++ -O3 -Wall -shared -std=c++14 -fPIC `python3 -m pybind11 --includes` bound_reader.cpp -o bound_reader`python3-config --extension-suffix` */

/*
	Try to debug by printing out all of the arguments used for a specific
	filterbank file, then hard-code those into a toy call where you can print
	stuff out.

	Anyway what's the deal with the current implementation? The zeroing
	might be my own idiocy but why can I not print anything afterward,
	like the whole program desists.
*/
py::array_t<float> read_sigproc_data(std::string filename, int header_size,
	int n_chan, int n_if, int n_int, int n_sel_chan,
	int chan_start_idx, int chan_stop_idx)
{ 
	//std::cout << "I think the data type has the following size" << sizeof(T);

	auto result = py::array_t<float>({n_int, n_if, n_sel_chan});

	// I don't know how much PyBind11 handles automatically;
	// will it be able to tell the difference between
	// an 8-bit int and a 32-bit float? Only tests will tell.
	int batch_size = n_sel_chan * sizeof(float);

	py::buffer_info buf = result.request();

	// We may have to get into some pointer arithmetic...
	char* index_ptr = (char*)buf.ptr;	

	std::ifstream file (filename, std::ios::in|std::ios::binary);
	//file.open(filename);
	std::streampos curs = file.tellg();
	
	int sizeFrac = sizeof(float) / sizeof(char);
	curs += 0; //self.t_start * sizeFrac  * n_if * n_chan;
	file.seekg(curs);

	if (file.is_open()) {
		#pragma omp parallel for
		for (int i = 0; i < n_int; i++)
			for (int j = 0; j < n_if; j++) {
				curs += sizeFrac * chan_start_idx;

				file.seekg(curs);

				//here it comes: pointer arithmetic
				int offset = i * n_if * n_sel_chan + j * n_int * n_sel_chan;
				// SUSPECT! also it might overflow

				file.read(index_ptr + offset, batch_size);				
				
				//char* dummy = new char[n_sel_chan]; 				
				//file.read(dummy, batch_size);	

				//std::cout << "index_ptr: " << index_ptr[offset] << offset << "\n";
				//std::cout << "dummy response: " << dummy[0] << "\n";

				curs += sizeFrac * (n_chan - chan_stop_idx);
				file.seekg(curs);
			}
		file.close();

		//std::cout << "the entire file content is in memory;" << 

			//"doesn't this present pressing hardware limitations?";
	}
	//else std::cout << "Unable to open file";


	return result;
}

/* Temporary quarantine while we figure out data types

// First let's try using templates to account for my
	// needing to return an array of a particular type
template <typename T>

// I am neglecting int_start_idx (setup of function instead)
	// and int_stop_idx (# loops instead)

// I am not yet convinced that I need filesize_in_bytes,
	// so long as I bound the loop correctly

//n_sel_chan was not explicitly advised, so I may be
	//doing something seriously incorrect

py::array_t<T> read_sigproc_data(std::string filename, int header_size,
	int n_chan, int n_if, int n_int, int n_sel_chan,
	int chan_start_idx, int chan_stop_idx)
{ 
	std::cout << "I think the data type has the following size" << sizeof(T);

	auto result = py::array_t<T>({n_int, n_if, n_sel_chan});

	// I don't know how much PyBind11 handles automatically;
	// will it be able to tell the difference between
	// an 8-bit int and a 32-bit float? Only tests will tell.
	int batch_size = n_sel_chan * sizeof(T);

	py::buffer_info buf = result.request();

	// We may have to get into some pointer arithmetic...
	char* index_ptr = (char*)buf.ptr;	

	std::ifstream file (filename, std::ios::in|std::ios::binary);
	file.open(filename);
	std::streampos curs = file.tellg();
	
	if (file.is_open()) {
		#pragma omp parallel for
		for (int i = 0; i < n_int; i++)
			for (int j = 0; j < n_if; j++) {
				file.seekg(curs + n_int * chan_start_idx);

				//here it comes: pointer arithmetic
				int offset = i * n_if + j * n_int * sizeof(T) / sizeof(char);
				//yeah, that last sizeof was infantile. Better safe than sorry

				file.read(index_ptr + offset, batch_size);				

				file.seekg(curs + n_int * (n_chan - chan_stop_idx));
			}
		file.close();

		std::cout << "the entire file content is in memory;" << 
			"doesn't this present pressing hardware limitations?";
	}
	else std::cout << "Unable to open file";

	return result;
}
*/

PYBIND11_MODULE(bound_reader, m) {
	m.doc() = "Read file using pybind11";

	m.def("add", &add, "Adds two numbers (diagnostic function)");

	m.def("read_sigproc_data", &read_sigproc_data, "File Reader (32-bit float)");

	/*
	// I am throwing out random primitives. I should probably find
	// a way to implement, for example, 
	m.def("read_sigproc_data", &read_sigproc_data<char>, "File Reader (8-bit integer");
	m.def("read_sigproc_data", &read_sigproc_data<int>, "File Reader (32-bit int, most likely"); //Don't forget that in C, size_of(int) is NOT universal
	
	m.def("read_sigproc_data", &read_sigproc_data<double>, "File Reader (64-bit float)");
	*/

	/* TODO:
		I am only overloading the RETURN type. How is the bind supposed to know
		which function to call?
			In all likelihood you will need to return with a data type parameter.
	*/
}
