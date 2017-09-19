import java.net.*; // for Socket
import java.io.*; // for IOException and Input/OutputStream
import java.util.Arrays;
import java.text.DecimalFormat;

public class ClientTCP {
	static byte requestID = 7;

	public static void main(String[] args) throws IOException {

		if (args.length != 4) // Test for correct # of args
			throw new IllegalArgumentException("Parameter(s): <client> <servername> <PortNumber> <Operation> <String>");

			String server = args[0]; // Server name or IP address

			// Convert input String to bytes using the default character encoding
			byte[] messageBuffer = args[3].getBytes();
			if (messageBuffer.length > 252)
				throw new IllegalArgumentException("Message can be no longer than 252 characters");

			int servPort = Integer.parseInt(args[1]);

			Integer totalMessageLength = 3 + messageBuffer.length;
			byte[] byteBuffer = new byte[totalMessageLength];
			
			if (!args[2].equals("5") && !args[2].equals("80") && !args[2].equals("10"))
				throw new IllegalArgumentException("Invalid operation requested. 5 - cLength, 80 - disemvoweling, 10 - uppercasing");
 			
			//Affix header bytes to buffer
			byteBuffer[0] = totalMessageLength.byteValue();
			byteBuffer[1] = requestID;
			//place operation byte	
			switch (args[2]) {
				case "5": byteBuffer[2] = 5;
					break;
				case "80": byteBuffer[2] = 80;
					break;
				case "10": byteBuffer[2] = 10;
					break;
				default:
			}
	
			//Append message to buffer
			for (int i = 0; i < messageBuffer.length; i++) {
				byteBuffer[i + 3] = messageBuffer[i];
			}

			// Create socket that is connected to server on specified port
			Socket socket = new Socket(server, servPort);
			System.out.println("Connected to server...sending echo string");
			InputStream in = socket.getInputStream();
			OutputStream out = socket.getOutputStream();
			out.write(byteBuffer); // Send the encoded string to the server
			
			long startTime = System.nanoTime();
				
			// Receive the response  back from the server
			int totalBytesRcvd = 0; // Total bytes received so far
			int bytesRcvd; // Bytes received in last read
			int receivedMessageLength; //Header byte for the length of received message
			int returnedRequestID; //RequestID returned by server
			
				
			while ((receivedMessageLength = in.read()) < 0){};
			receivedMessageLength = receivedMessageLength & 0xFF;
			
			System.out.println("Received message length: " + receivedMessageLength);
			byte[] responseBuffer = new byte[receivedMessageLength - 1]; //not accounting for the byte already read
			while (totalBytesRcvd < responseBuffer.length) {
				if ((bytesRcvd = in.read(responseBuffer, totalBytesRcvd, 
				responseBuffer.length - totalBytesRcvd)) == -1)
					throw new SocketException("Connection close prematurely");

				totalBytesRcvd += bytesRcvd;
			}
			
			returnedRequestID = responseBuffer[0] & 0xFF;
			if (args[2].equals("5"))
				System.out.println("RequestID: " + returnedRequestID + " Received: " + responseBuffer[1]); 
			else
				System.out.println("RequestID: " + returnedRequestID + " Received: " 
					+ new String(Arrays.copyOfRange(responseBuffer, 1, responseBuffer.length)));
			DecimalFormat frmt = new DecimalFormat("#.000");
			long difference = System.nanoTime() - startTime;
			System.out.println("Round trip time: " + frmt.format(difference * 0.000001) + " milliseconds");
			socket.close(); // Close the socket and its streams
	}
}
