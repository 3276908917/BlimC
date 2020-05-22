"""
# test_h52fil
"""

import pytest

import os
import blimpy as bl
from tests.data import voyager_h5


def test_h52fil_conversion():
    ''' Tests the conversion of fil files into h5 in both light and heavy modes.
    '''

    # Creating test file.
    bl.h52fil.make_fil_file(voyager_h5, new_filename='test.fil')

    # Creating a "large" test file.
    bl.h52fil.make_fil_file(voyager_h5, new_filename='test_large.fil', max_load=0.001)

    # Testing filename
    bl.h52fil.make_fil_file(voyager_h5, new_filename='test')

    # Deleting test file
    os.remove('test.fil')
    os.remove('test_large.fil')

def test_help():
    '''
    The user of these tests should verify that the help statement
    was printed as expected; this test merely verifies that the
    system exited.
    '''
    with pytest.raises(SystemExit):
        bl.h52fil.cmd_tool(['-h'])

'''
Okay, congratulations and all; here is your gold medal.
However: the valuable test is one which checks cmd_tool functionality
    in a way OTHER than just making sure that it exits!
    Yeah, I get it, you can catch syntax errors (...right?)
    But otherwise the cmd_tool is going to issue a SystemExit
    regardless of your actual input. So your current tests
    are fairly bad!
'''

def test_cmd_tool():
    '''
    This is the same large test file, but now through the cmd tool.
    '''
    #with pytest.raises(SystemExit):
    bl.h52fil.cmd_tool([voyager_h5, '-n', 'cmd.fil', '-l', '.001'])

def test_no_args():
    '''
    The cmd tool needs to exit, mandating a file name.
    '''
    with pytest.raises(SystemExit):
        bl.h52fil.cmd_tool()

if __name__ == "__main__":
    test_h52fil_conversion()
