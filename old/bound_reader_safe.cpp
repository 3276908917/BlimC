#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

/*c++ -O3 -Wall -shared -std=c++14 -fPIC -fopenmp `python3 -m pybind11 --includes` bound_reader.cpp -o bound_reader`python3-config --extension-suffix` */

namespace py = pybind11;

// This is only a diagnostic function, to ensure
	// that the binding was performed properly.
int add(int i, int j) {
    return i + j;
}

/*
	These parameters have not yet been implemented. Review what
	Danny Price said about them:
		filesize_in_bytes
		int_start_idx
		int_stop_idx
*/

py::array_t<float> read_sigproc_data(std::string filename, int data_index,
	int n_chan, int n_if, int n_int, int n_sel_chan,
	int chan_start_idx, int chan_stop_idx, unsigned char dtype_size)
{ 
	auto result = py::array_t<float>({n_int, n_if, n_sel_chan});

	py::buffer_info buf = result.request();

	char* index_ptr = (char*)buf.ptr;	

	std::ifstream file (filename, std::ios::in|std::ios::binary);
	std::streampos curs = file.tellg();

	curs += data_index; //+ self.t_start * sizeFrac * n_if * n_chan;
	file.seekg(curs);

	int jScale = n_sel_chan * dtype_size; // this is also the size of a single batch of data to be read
	int iScale = n_if * jScale;

	if (file.is_open()) {
		#pragma omp parallel for
		// this might actually be worthless since we use hard disks
		for (int i = 0; i < n_int; i++)
			for (int j = 0; j < n_if; j++) {
				curs += dtype_size * chan_start_idx;
				file.seekg(curs);

				int offset = i * iScale + j * jScale;
				// this might overflow

				file.read(index_ptr + offset, jScale);				

				curs += dtype_size * (n_chan - chan_stop_idx);
				file.seekg(curs);
			}
		file.close();
	}
	else {
		std::cout << "There was an error and the file could not be opened";
		// You really should try something more professional, like throwing a formal error
	}

	return result;
}


PYBIND11_MODULE(bound_reader, m) {
	m.doc() = "Read file using pybind11";

	m.def("add", &add, "Adds two numbers (diagnostic function)");

	m.def("read_sigproc_data", &read_sigproc_data, "File Reader (32-bit float)");

	/* TODO:
		I am only overloading the RETURN type. How is the bind supposed to know
		which function to call?
			In all likelihood you will need to return with a data type parameter.
	*/
}
