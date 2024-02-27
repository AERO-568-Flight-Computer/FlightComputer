import unittest
from StirlingNGI import *


class TestStirlingNGI(unittest.TestCase):
    def setUp(self) -> None:
        self.ngi = StirlingInceptor()

    def test_msg00(self):
        """ Message 0 - Initialisation Message PORT 7000 """
        ans_pitch = b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x42\x47\x72\x79\xbd\x27\xa0\x72\xc1\xfb\xcb\x6e\xbd\x34\x80\xad\x41\x01\x00\x00\x00"
        msg01 = self.ngi.msg00(axis='pitch')
        self.assertEqual(ans_pitch, msg01)

        ans_roll = b"\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x42\x47\x72\x79\xbd\x27\xa0\x72\xc1\xfb\xcb\x6e\xbd\x34\x80\xad\x41\x01\x00\x00\x00"
        msg01 = self.ngi.msg00(axis='roll')
        self.assertEqual(ans_roll, msg01)

    def test_msg01(self):
        """ Message 1 - Control Message PORT 7001 """
        # IBIT Initiate Message - IBIT on followed by IBIT off
        ans_IBIT = b"\x01\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        msgIBIT = self.ngi.msg01(IBIT=True)
        self.assertEqual(ans_IBIT, msgIBIT)

        ans_IBIT = b"\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        msgIBIT = self.ngi.msg01(IBIT=False)
        self.assertEqual(ans_IBIT, msgIBIT)

    def test_msg02(self):
        """ Message 2 - Rotary Characteristic Message PORT 7002 """
        ans_rot = b"\x02\x00\x00\x00\x00\x00\x00\x00" \
                  b"\x00\x00\x00\x00\x55\x55\x55\x41" \
                  b"\x55\x55\x85\x40\x55\x55\xd5\x41" \
                  b"\x55\x55\x05\x41\x00\x00\x20\x42" \
                  b"\x00\x00\x48\x41\x55\x55\x55\x42" \
                  b"\x55\x55\x85\x41\x55\x55\x85\x42" \
                  b"\x00\x00\x00\x80\x55\x55\x55\x41" \
                  b"\xd2\xa6\x72\x40\xe2\xe8\xfd\x41" \
                  b"\x55\x55\x05\x41\x00\x00\x20\x42" \
                  b"\x00\x00\x48\x41\x55\x55\x55\x42" \
                  b"\x55\x55\x85\x41\x55\x55\x85\x42"

        PosCoords = [[0, 13.333333015441895],
                     [4.166666507720947, 26.66666603088379],
                     [8.333333015441895, 40.0],
                     [12.5, 53.33333206176758],
                     [16.66666603088379, 66.66666412353516]]
        NegCoords = [[-0.0, 13.333333015441895],
                     [3.7914319038391113, 31.738712310791016],
                     [8.333333015441895, 40.0],
                     [12.5, 53.33333206176758],
                     [16.66666603088379, 66.66666412353516]]
        msg_rot = self.ngi.msg02(PosCoords, NegCoords)
        self.assertEqual(ans_rot, msg_rot)

    def test_msg05(self):
        ans = b"\x05\x00\x00\x00\xbb\x74\x13\x3c\x5d\x31\x66\x3f"
        msg_mod = self.ngi.msg05('pitch')
        self.assertEqual(ans, msg_mod)

    def test_msg06(self):
        ans = b"\x06\x00\x00\x00\x00\x00\x80\x3f\x00\x00\x00\x00"
        msg_shaker = self.ngi.msg06('pitch')
        self.assertEqual(ans, msg_shaker)

    def test_msg07(self):
        ans = b"\x07\x00\x00\x00\x00\x00\x00\x00"
        msg_forcebias = self.ngi.msg07('pitch')
        self.assertEqual(ans, msg_forcebias)

    def tearDown(self) -> None:
        self.ngi.tearDown()


