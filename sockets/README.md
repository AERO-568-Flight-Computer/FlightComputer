## Demo UDP client and server:
### To run this demo:
Open a terminal and run: python udp_server.py<BR>
Open a 2nd terminal on the same computer and run: python udp_client.py<P>
When you type into the client, the message will be displayed on the server.

I've commented it pretty well, but I imagine there is some terminology that you might not know.  I'm learning to use Github Copilot. It will be able to answer your questions easily and explain the code. I used copilot to develop this example.

My suggestions to expand this code:
- Integrate the voltz_actuator code into the server so it will actually move the servo. Then, you can move the servo from the UDP client over a UDP socket connection.
- Implement unit testing on the UDP client. Test for range of values, type of input (strings, integers, other types), and any other test you can imagine.
- Think about how we will make the "master" or "time master" as Paulo/Carl call it. Will we have a single UDP server and many UDP clients?
