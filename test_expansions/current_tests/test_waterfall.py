from tests.data import voyager_h5
import blimpy as bl
import os

import pytest
#with pytest.raises(RuntimeError):

def test_info():
    a = bl.Waterfall(voyager_h5)
    print(a)
    a.info()
    a.blank_dc(n_coarse_chan=1)
    a.calibrate_band_pass_N1()
    'Below: orphaned functions. That is a little strange.'
    a.grab_data()
    a.read_data()
    'It is VERY strange for internal functions to be orphaned...'
    a._get_chunk_dimensions()
        # plenty of missed if suites
    a._get_blob_dimensions((300, 300, 300, 300))
    a._update_header()

def test_cmdline():
    from blimpy.waterfall import cmd_tool

    args = [voyager_h5, '-s', 'test.png']
    cmd_tool(args)

    args = [voyager_h5, '-i']
    cmd_tool(args)

    if os.path.exists('test.h5'):
        os.remove('test.h5')
        args = [voyager_h5, '-H', '-o', 'test.h5']
        cmd_tool(args)
        assert os.path.exists('test.h5')
        os.remove('test.h5')

    if os.path.exists('test.fil'):
        os.remove('test.fil')
        args = [voyager_h5, '-F', '-o', 'test.fil']
        cmd_tool(args)
        assert os.path.exists('test.fil')
        os.remove('test.fil')

# Again, hash comments will generally indicate
# that a test did not increase coverage.
def test_cmd_arguments():
    from blimpy.waterfall import cmd_tool
    
    args = [voyager_h5, '-H', '-F', '-o', 'test.fil']
    with pytest.raises(Warning):
        cmd_tool(args)

    #args = [voyager_h5, '-D', '-p', 'a']
    #cmd_tool(args)

if __name__ == "__main__":
    test_info()
    test_cmdline()

