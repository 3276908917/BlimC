import blimpy as bl
from tests.data import voyager_fil

# '&' indicates tests which only check for
    # runtime errors and do not verify the accuracy
    # of any outputs thereof

# &
def test_size():
    bl.match_fils.find_header_size(voyager_fil)

def test_reset_works():
    """
    The test is trivial, but coverage must be
    increased at all costs.
    """
    a, b = bl.match_fils.reset_outs()
    assert a is None
    assert b is None

# &
def test_batcher():
    """
    This test may be obsolete once/if test_cmd
        can be patched up.
    """
    bl.match_fils.make_batch_script()

'''
Decommissioned: the test cannot be run universally...
'''
#def test_cmd():
#    args = [voyager_fil, voyager_fil]
#    bl.match_fils.cmd_tool(args)

