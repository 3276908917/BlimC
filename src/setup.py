"""
setup.py -- setup script for use of packages.
"""
from setuptools import setup, find_packages, Extension
#import pybind11

__version__ = '1.4.1'

with open("README.md", "r") as fh:
    long_description = fh.read()

# create entry points
# see http://astropy.readthedocs.org/en/latest/development/scripts.html
entry_points = {
    'console_scripts' : [
        'filutil = blimpy.filterbank:cmd_tool',
        'watutil = blimpy.waterfall:cmd_tool',
        'rawutil = blimpy.guppi:cmd_tool',
        'fil2h5 = blimpy.fil2h5:cmd_tool',
        'h52fil = blimpy.h52fil:cmd_tool',
        'matchfils = blimpy.match_fils:cmd_tool',
        'bldice = blimpy.dice:cmd_tool'
     ]
}

install_requires = [
        'matplotlib<3.0;python_version=="2.7"',
        'matplotlib;python_version>"2.7"',
        'astropy<3.0;python_version=="2.7"',
        'astropy;python_version>"2.7"',
        'numpy',
        'cython',
        'h5py',
        'scipy',
        'six',
		'pybind11'
]

extras_require = {
        'full': [
            'bitshuffle',
            'pyslalib',
        ]
}

# Credit to Sylvain Corlay

class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)

read_module = Extension(
    "bound_reader",
    ['blimpy/io/bound_reader.cpp'],
	include_dirs=[
		get_pybind_include(),
		get_pybind_include(user=True)
	],
    language           = 'c++',
    extra_compile_args = ['-std=c++14', '-fPIC', '-fopenmp'],
    #define_macros      = [('DOCTEST_CONFIG_DISABLE',None)]
  )

setup(name='blimpy',
      version=__version__,
      description='Python utilities for Breakthrough Listen SETI observations',
      long_description=long_description,
      long_description_content_type='text/markdown',
      platform=['*nix'],
      license='BSD',
      install_requires=install_requires,
      extras_require=extras_require,
      url='https://github.com/ucberkeleyseti/blimpy',
      author='Danny Price, Emilio Enriquez, Yuhong Chen, Mark Siebert, and BL contributors',
      author_email='dancpr@berkeley.edu',
      entry_points=entry_points,
      packages=find_packages(),
      zip_safe=False,
      classifiers=[
          'Development Status :: 5 - Production/Stable',
          'Environment :: Console',
          'Natural Language :: English',
          'Operating System :: POSIX :: Linux',
          'Programming Language :: Python :: 2.7',
          'Intended Audience :: Science/Research',
          'License :: OSI Approved :: BSD License',
          'Topic :: Scientific/Engineering :: Astronomy',
      ],
      setup_requires=['pytest-runner'],
      tests_require=['pytest'],
      test_suite="blimpytests",
      ext_modules = [read_module]
)
