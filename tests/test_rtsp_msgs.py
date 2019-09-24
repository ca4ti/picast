
import socket
import threading
from time import sleep

from picast.picast import PiCast
from picast.video import WfdVideoParameters

import pytest


class MockPlayer():
    def __init__(self):
        pass

    def start(self):
        pass

    def stop(self):
        pass


class MockRtspServer(threading.Thread):

    def __init__(self, port):
        super(MockRtspServer, self).__init__()
        self.port = port

    def run(self):
        self.sock =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind(('127.0.0.1', self.port))
        self.sock.listen(1)
        conn, remote = self.sock.accept()

        # M1
        m1 = "OPTION * RTSP/1.0\r\n\r\n"
        conn.sendall(m1.encode("UTF-8"))
        m1_resp = conn.recv(1000).decode("UTF-8")
        assert m1_resp == "RTSP/1.0\r\nCSeq: 1\r\nPublic: org.wfs.wfd1.0, SET_PARAMETER, GET_PARAMETER\r\n\r\n"

        # M2
        m2 = conn.recv(1000).decode("UTF-8")
        assert m2 == "OPTIONS * RTSP/1.0\r\nCSeq: 100\r\nRequire: org.wfs.wfd1.0\r\n\r\n"
        m2_resp = "RTSP/1.0 200 OK\r\nCSeq: 100\r\nPublic: org.wfa.wfd1.0, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER\r\n\r\n"
        conn.sendall(m2_resp.encode("UTF-8"))

        # M3
        m3 = "GET_PARAMETER rtsp://localhost/wfd1.0 RTSP/1.0\r\nCSeq: 2\r\nContent-Type: text/parameters\r\n" \
             "Content-Length: 141\r\n\r\n" \
             "wfd_video_formats\r\nwfd_audio_codecs\r\nwfd_3d_video_formats\r\nwfd_content_protection\r\n" \
             "wfd_display_edid\r\nwfd_coupled_sink\r\nwfd_client_rtp_ports\r\n\r\n"
        conn.sendall(m3.encode("UTF-8"))
        m3_resp = conn.recv(1000).decode("UTF-8")
        assert m3_resp == "RTSP/1.0 200 OK\r\nCSeq: 2\r\nContent-Type: text/parameters\r\nContent-Length: 289\r\n\r\n" \
                          "wfd_client_rtp_ports: RTP/AVP/UDP;unicast 1028 0 mode=play\r\n" \
                          "wfd_video_formats: 00 00 01 01 00000001 00000000 00000000 00 0000 0000 00 none none\r\n" \
                          "wfd_audio_codecs: LPCM 00000003 00\r\nwfd_3d_video_formats: none\r\n" \
                          "wfd_content_protection: none\r\nwfd_display_edid: none\r\nwfd_coupled_sink: none\r\n\r\n"

        # M4
        m4 = "SET_PARAMETER rtsp://localhost/wfd1.0 RTSP/1.0\r\nCSeq: 3\r\nContent-Type: text/parameters\r\nContent-Length: 302\r\n\r\n" \
             "wfd_video_formats: 00 00 01 01 00000001 00000000 00000000 00 0000 0000 00 none none\r\nwfd_audio_codecs: LPCM 00000002 00\r\n" \
             "wfd_presentation_URL: rtsp://192.168.173.80/wfd1.0/streamid=0 none\r\n" \
             "wfd_client_rtp_ports: RTP/AVP/UDP;unicast 1028 0 mode=play"
        conn.sendall(m4.encode("UTF-8"))
        m4_resp = conn.recv(1000).decode("UTF-8")
        assert m4_resp == "RTSP/1.0 200 OK\r\nCSeq: 3\r\n\r\n"

        # M5
        m5 = "SET_PARAMETER rtsp://localhost/wfd1.0 RTSP/1.0\r\n" \
             "CSeq: 4\r\nContent-Type: text/paramters\r\nContent-Length: 27\r\nwfd_trigger_method: SETUP\r\n\r\n"
        conn.sendall(m5.encode("UTF-8"))
        m5_resp = conn.recv(1000).decode("UTF-8")
        assert m5_resp == "RTSP/1.0 200 OK\r\nCSeq: 4\r\n\r\n"

        # M6
        m6 = conn.recv(1000).decode("UTF-8")
        assert m6 == "SETUP rtsp://192.168.173.80/wfd1.0/streamid=0 RTSP/1.0\r\nCSeq: 101\r\n" \
                     "Transport: RTP/AVP/UDP;unicast;client_port=1028\r\n\r\n"
        m6_resp = "RTSP/1.0 200 OK\r\nCSeq: 101\r\nSession: 6B8B4567;timeout=30\r\n" \
                  "Transport: RTP/AVP/UDP;unicast;client_port=1028;server_port=5000\r\n\r\n"
        conn.sendall(m6_resp.encode("UTF-8"))

        # M7
        m7 = conn.recv(1000).decode("UTF-8")
        assert m7 == "PLAY rtsp://192.168.173.80/wfd1.0/streamid=0 RTSP/1.0\r\nCSeq: 102\r\nSession: 6B8B4567\r\n\r\n"
        m7_resp = "RTSP/1.0 200 OK\r\nCSeq: 102\r\n\r\n"
        conn.sendall(m7_resp.encode("UTF-8"))


class FreePort:
    # this class is Borg/Singleton
    _shared_state = {
        '_port': None,
        '_lock': threading.Lock()
    }

    def __new__(cls, *p, **k):
        self = object.__new__(cls, *p, **k)
        self.__dict__ = cls._shared_state
        return self

    def __init__(self):
        if self._port is None:
            with self._lock:
                if self._port is None:
                    s = socket.socket(socket.AF_INET, type=socket.SOCK_STREAM)
                    s.bind(('localhost', 0))
                    address, port = s.getsockname()
                    s.close()
                    self._port = port

    @property
    def port(self):
        return self._port


@pytest.fixture
def rtsp_mock_server():
    port = FreePort().port
    return MockRtspServer(port)


@pytest.mark.basic
def test_rtsp_connection(monkeypatch, rtsp_mock_server):

    def mockretrun(self, sock, remote, port):
        assert remote == '192.168.173.80'
        assert port == 7236
        free_port = FreePort().port
        sock.connect(('127.0.0.1', free_port))
        return True

    def videomock(self):
        return "wfd_video_formats: 00 00 01 01 00000001 00000000 00000000 00 0000 0000 00 none none\r\n" \
               "wfd_audio_codecs: LPCM 00000003 00\r\nwfd_3d_video_formats: none\r\n" \
               "wfd_content_protection: none\r\nwfd_display_edid: none\r\nwfd_coupled_sink: none\r\n\r\n"

    monkeypatch.setattr(PiCast, "connect", mockretrun)
    monkeypatch.setattr(WfdVideoParameters, "get_video_parameter", videomock)

    rtsp_mock_server.start()
    sleep(0.5)
    player = MockPlayer()
    picast = PiCast(player)
    picast.start()
    picast.join()
