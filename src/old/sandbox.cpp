/** Other ideas for apocalypse.txt

	How much pain and suffering comes with bitshuffle. I can explain what I tried to do,
	but I never came to a solution. I was always stuck with the plugin, which
	is a serious obstacle because it disallows writing, which is integral to the
	proper function of the Blimpy file handling code.

	How to install and configure PyBind for your system
		* In my case, the IDEs almost universally rejected PyBind,
			and I ended up mostly having to check syntax on my own.
		* If you are using a package-handler (e.g. Anaconda) to 
			handle your Astropy, Blimpy, NumPy, etc.,
			make sure that you activate your environment before compiling.

		* example compile statement

c++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` elections.cpp -o elect`python3-config --extension-suffix`

		* This example document of simpler ways to use PyBind. In other
			words, if you are not as strong in type casting and template
			making, I'll wager this document will prove far less scary
			to you than some of the official PyBind tutorials.
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
	auto result = py::array_t<double>(3);
	py::buffer_info buf = result.request();

	double* bp = (double*)buf.ptr;

	bp[0] = 3.23;
	bp[1] = 4.96;
	bp[2] = 7.00;

	return result;
}

/* The following two code blocks demonstrate the definition
	of an arbitrary struct which can then be used as the 
	template for a NumPy-compatible array
*/

struct Decade {
	int x[10];
	double y;
};

py::array_t<Decade> magic_decade() {
	auto result = py::array_t<Decade>(3);

	// I really should have demonstrated the specific functionality of the
	// struct, but oh well.

	return result;
}

// Demonstration of the creation from the CPP side of a two-dimensional array

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

/* Todo: purge CPU freq. Make sure none of the LXDE is still lying about.*/

//Overloading test
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

// you might still get killed by arbitrary data types.
		// It's going to force me into templates, which will sink at least
		// another week into the learning curve.
// I have to find some way to dumb-down the code. If I get a data type,
	// all I care about is the # bytes allocated to a variable of that type

	//Other ideas: look at how this memory is freed. Won't it crash the
		// system if I keep requesting random cubes?
	// >> I am fairly sure that PyBind automatically handles this already


/*
py::array_t<Matrix> magic_matrix() {
	auto result = py::array_t<Matrix>(3);
	py::buffer_info buf = result.request();

	double** bp = (double**)buf.ptr;

	bp[0][0] = 3.23;
	bp[1][0] = 4.96;
	bp[2][0] = 7.00;

	bp[0][1] = -1;
	bp[1][1] = 1;
	bp[2][1] = 0;

	bp[0][2] = 3;
	bp[1][2] = 9;
	bp[2][2] = 2.71828182859;

	return result;
}
*/

PYBIND11_MODULE(elect, m) {
    //m.doc() = "pybind11 example plugin"; // optional module docstring
	m.def("add", &add, "Adds two numbers");
	
	PYBIND11_NUMPY_DTYPE(Decade, x, y);

	m.def("magic_array", &magic_array, "Bequeathe a hard-coded array");
	m.def("magic_decade", &magic_decade, "Grant a hard-coded 3x3");
	m.def("magic_matrix", &magic_matrix, "Grant a hard-coded 3x3");

	// I am using a feature here which, according to the PyBind11 website, requires c++14, rather than c++11
	m.def("random_cube", (py::overload_cast<int, int, int, int> (&random_cube)), "Grant a hard-coded 3x3");
	m.def("random_cube", (py::overload_cast<int, int, int, double> (&random_cube)));
}

// Graveyard

/* numpy tutorial
class Matrix {
public:
	Matrix(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
		m_data = new float[rows*cols];
	}
	float *data() { return m_data; }
	size_t rows() const { return m_rows; }
	size_t cols() const { return m_cols; }
private:
	size_t m_rows, m_cols;
	float *m_data;
};

py::class_<Matrix>(m, "Matrix", py:buffer_protocol())
	.def_buffer([](Matrix &m) -> py::buffer_info {
		return py::buffer_info(
			m.data(),
			sizeof(float),
			py::format_descriptor<float>::format(),
			2,
			{ m.rows(), m.cols() },
			{ sizeof(float) * m.cols(),
			sizeof(float) }
		);
	});
*/
