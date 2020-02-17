/** example compile statement
	c++ -O3 -Wall -shared -std=c++17 -fPIC `python3 -m pybind11 --includes` array_examples.cpp -o array_examples`python3-config --extension-suffix`
*/

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// This one's straight from the PyBind GitHub
int add(int i, int j) {
    return i + j;
}

// Demonstration of one-dimensional array crafting
// Compatible with NumPy, of course
py::array_t<double> magic_array() {
	// This line creates the container for the results,
	// but we cannot edit it directly
	auto result = py::array_t<double>(3);

	// We also have to create a buffer...
	py::buffer_info buf = result.request();

	// ...and then a pointer to that buffer.
	double* bp = (double*)buf.ptr;

	// Finally, we are ready to modify the contents of 'result'
	bp[0] = 3.23;
	bp[1] = 4.96;
	bp[2] = 7.00;

	return result;
}

/** The following two code blocks demonstrate the definition
	of an arbitrary struct which can then be used as the 
	template for a NumPy-compatible array
*/

struct Decade {
	int x[10];
	double y;
};

py::array_t<Decade> magic_decade() {
	auto result = py::array_t<Decade>(3);

	return result;
}

/** Demonstration of the creation of a two-dimensional array
	This is helpful for file reading for example,
 	when you do not want to wait on the Python script for an empty array.
*/
py::array_t<double> magic_matrix() {
	auto result = py::array_t<double>({2, 3});	

	py::buffer_info buf = result.request();

	double* bp = (double*)buf.ptr;

	/* It's awkward, but I don't know of any other way to get two dimensions
		than by compressing all data into one dimension for most of the code,
		(i.e. all elements accessible from a single index) then reshaping
		at the end, so that NumPy properly acknowledges its shape.
	*/	

	bp[0] = 1.1;
	bp[1] = 2.2;
	bp[2] = 3.3;

	bp[3] = 4.4;
	bp[4] = 5.5;
	bp[5] = 6.6;

	result.resize({2, 3});

	return result;
}


/** This is a demonstration of extending the ideas used so far.
	It returns a pseudorandomized 3D array of integers.
*/
py::array_t<int> random_cube(int rows, int columns, int echelons, int dtype) {
	auto result = py::array_t<int>({rows, columns, echelons});

	py::buffer_info buf = result.request();

	int* bp = (int*)buf.ptr;

	for (long long int c = 0; c < rows * columns * echelons; c++) {
		bp[c] = rand() % 100; //random integers. Yawn
	}

	result.resize({rows, columns, echelons});

	return result;
}

/** Overloading with pybind is perfectly acceptable, but now you will
	have to declare two separate functions of the same name
	(see follow-up comment at the end).
*/
py::array_t<double> random_cube(int rows, int columns, int echelons, double dtype) {
	auto result = py::array_t<double>({rows, columns, echelons});

	py::buffer_info buf = result.request();

	double* bp = (double*)buf.ptr;

	for (long long int c = 0; c < rows * columns * echelons; c++) {
		bp[c] = (double)(rand() % 100) + ((double) (rand() % 100) / 100.);
	}

	result.resize({rows, columns, echelons});

	return result;
}

/** In some cases you will not be permitted to use pointers of a specific
	data type. The following function is a demonstration of the accurate
	and fluid casting that may be performed to get around these limitations.
*/
py::array_t<int> binary_array() {
	auto result = py::array_t<int>(3);
	py::buffer_info buf = result.request();

	char* bp = (char*)buf.ptr;

	bp[0] = 3;
	bp[1] = 4;
	bp[2] = 5;
	bp[3] = 6;

	bp[4] = 123;
	bp[5] = 124;
	bp[6] = 125;
	bp[7] = 126;

	bp[8] = 81;
	bp[9] = 9;
	bp[10] = 90;
	bp[11] = 99;

	return result;
}

/** Templates may be necessary in some cases, but they are difficult to 
	attach to Numpy arrays.
*/
template <typename T>
T subtract(T x, T y) {
	return x - y;
}

PYBIND11_MODULE(array_examples, m) {
    //m.doc() = "pybind11 example plugin"; // optional module docstring
	m.def("add", &add, "Adds two numbers");
	
	// This is a special statement necessary for numpy to accept
		// arrays of a user-defined struct.
	PYBIND11_NUMPY_DTYPE(Decade, x, y);

	m.def("magic_array", &magic_array, "Hard-coded array");
	m.def("magic_decade", &magic_decade);
	m.def("magic_matrix", &magic_matrix, "Hard-coded 2D array");

	// I am using a feature here which, according to the PyBind11 website, requires c++14, rather than c++11
	m.def("random_cube", (py::overload_cast<int, int, int, int> (&random_cube)), "Returns a pseudo-randomized tensor of integers");
	m.def("random_cube", (py::overload_cast<int, int, int, double> (&random_cube)), "Returns a pseudo-randomized tensor of doubles");

	// To use templates, you have to bind every type that you plan to use
	m.def("subtract", subtract<double>);
	m.def("subtract", subtract<int>);
	m.def("subtract", subtract<float>);
	m.def("subtract", subtract<char>);
	//Hm. I forgot & but the code still works. Not sure about this one.

	m.def("binary_array", &binary_array);
}

