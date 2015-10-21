package odl_ut;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class parsePacket {
    public static void main(String argv[]) throws Exception {

        byte buffer[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	int varInt;

	if (argv[0].length() < 5) {
        	System.out.print("Smaller string\n");
		return;
	}

	varInt = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];

        System.out.print("Data Input int[0]: 0x" + Integer.toHexString( varInt ));
        System.out.print("\n");
    }
}
