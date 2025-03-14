1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a commands's output is fully recieved from the server by using techniques such as delimiters, message size/length and EOF markers. By using these techniques, partial reads and complete message transmission is ensured by allowing the client to detect when the entire response has been received.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol defines and detected the beginning and end of a command sent over a TCP connection by using explicit delimiters like \n and/or message length/frame. If message boundaries are not handled correctly, commands may be partially read or misinterpreted, be unintentionally split and/or lead to protocol corruption.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain session info between requests, which allows for continuous interaction. In comparison, stateless protocols do not store past interactions, making each request independent.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Despite the fact that UDP is "unreliable", it is faster and more efficient than TCP for applications where that require performance in real time. An example where it is used are online video games.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The interface/abstraction that is provided by the operating system to enable applications to use network communicatons is the socket API.