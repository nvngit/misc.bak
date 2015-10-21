package odl_ut;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 * @author alexis
 */
public class capwapWTP {
    final static int PORT = 5246;

    public static void main(String argv[]) throws Exception {

        byte buffer[] = {0x00, 0x20, 0x01, 0x02, 0x03, 0x04, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

        if (argv.length != 1) {
		System.out.print("Usage: capwapWTP <AC_IP>\n");
		return;
	}

        InetAddress server = InetAddress.getByName(argv[0]);
        //int length = buffer.length();
        int length = 18;

        System.out.print("Sending CAPWAP Discovery packet to AC IP: " + argv[0] + "\n");

        DatagramPacket dataSent = new DatagramPacket(buffer, length, server, PORT);
        DatagramSocket socket = new DatagramSocket();

        socket.send(dataSent);
    }
}

