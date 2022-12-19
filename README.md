# Computer-Network (2022 - 2 / 최형기 교수님 )
## 2022년 2학기 컴퓨터 네트워크 개론 프로젝트
<hr/>

### 1. Stripping IP Packets
이 프로젝트는 Wireshark 에서 잡은(trace) packets들에 해당하는 정보들을 pcap파일로 저장한후,
해당 pcap파일을 코드로 불러와서 packet 들의 정보들을 parsing하는 프로젝트이다.
아래 글은 프로젝트 과제에서 고려해야할 사항이다.

<hr/>

>Format of Trace (PCAP formatted)
The first 24 bytes is for the file information. You may safely ignore this file header. Each packet is encapsulated multiple packet headers. The outmost one is 16-byte pcap_pkthdr. The format of this header is shown in Figure 1. The struct timeval sec refers to the timestamp that packet was recorded into the trace in second. The struct timeval usec refers to the timestamp that packet was recorded into the trace in micro-second. The caplen refers to the length of the packet presented in the trace. The len refers to the actual length of the packet. Headers in the next are the Ethernet header and IP header. The formats of these headers are available in elsewhere. 
References http://www.tcpdump.org. <br/>
![packets](https://user-images.githubusercontent.com/95288696/208339944-91a33a26-d438-4311-a4a8-8a2689370c85.png)<br/>
Figure 1: The format of pcap_pkthdr
<br/>

> Parsing Information
> 1.	At least 500 packets
> 2.	IPv4
> 3.	TCP, UDP, ICMP
> 4.	Five different applications running over TCP and 2 different applications running over UDP
> 5.	At least 10 fragmented packets (using special applications or commands)
> 6.	TCP options
> You need to parse the following information in the trace.
> 7.	Local time that was recorded in the format of hh:mm:ss.xxxxxx where xxxxxx refers to the micro second.
> 8.	The captured length, actual length and the length in the IP header.
> 9.	Source MAC address → Destination MAC address. The format of the MAC address should be XX:XX:XX:XX:XX:XX.
> 10.	Source IP address → Destination IP address. The IP address must be in the dotted decimal format.
> 11.	A protocol presented in the IP payload.
> 12.	Identification in decimal and Flags in either DF or MF.
> 13.	TTL
> 14.	Type of Service
> Word of Caution) Please pay attention to the Endian. Depending upon the endian type the way you parse the information should be different. Verify your result against Wireshark.

<hr/>

#### 코드 요약 정보

패킷파일의 헤더에 포함된 24바이트를 fseek 함수로 파일 읽기 위치를 뒤로 옮긴 후에 parsing을 진행한다.<br/>
그 다음부터 순차적으로 2계층 3계층 헤더 (각, Ethernet header 와 IP header) 정보들을 순차적으로 파싱한다.<br/>
헤더는 앞에 차곡차곡 붙어져 감을 잊지말자.<br/>
데이터의 수신과정으로, 1계층의 physical layer 부터 순차적으로 헤더를 떼어내므로,<br/>
2,3,4,5계층 헤더 순서대로 패킷 헤더에 저장되어 있을 것이다.<br/>
따라서 2, 3계층 순으로 헤더 파싱을 진행한다.<br/>
그 이후에, 앞서 파싱했던 pkthdr (패킷 헤더)에 포함되어 있는 length 정보를 바탕으로, 2,3 계층 헤더 정보를 파싱한다.<br/>
그 후 해당 length만큼 파일 읽기 위치를 옮긴다.<br/>
이 때에도 16바이트를 추가함을 잊지 말아야 하겠다.

<hr/>

### 2. IPv4 Socket Programming
이 프로젝트는 Socket Application Programming Interface 인 Socket API를 이용하여 remote process 와 local process 간의 통신을 하는 프로젝트이다. <br/>
요약하자면, 성균관대학교 랩 서버와 정보를 주고받는데, local 에서도 client와 server를 만들어서 진행된다. 다음은 단계별 통신 과정을 요약한 것이다.
1. local에서 client 소켓 프로그래밍을 진행하여, 랩실 서버 (115.145.170.185 = 2022fcn.ddns.net) 과 통신하면서 정보를 주고받기
2. 서버측에게 전송할 정보는 1. 학번 2. 내가 만든 서버의 IP 주소, 3. 내가 만든 서버의 포트 넘버 4. 확인 5. 확인 6. 확인 7. 랩 서버에서 나의 서버에게 전송한 토큰넘버이다.
3. 랩서버는 해당 정보들을 바탕으로, 클라이언트 5개를 생성한 후, 내가 만든 local 서버에게 random 한 토큰 넘버 5개를 무작위로 전송한다.
4. 내가 만든 local 서버는 다시 내가 만든 client로 해당 토큰 넘버를 전송한다.
5. client는 토큰 넘버를 잘 받고, 다시 랩서버로 전송한다.
6. 랩 서버는 토큰 넘버가 일치하는지 확인하는 과정을 거치고, 확인 메세지를 클라이언트로 전송한다.
<br/>
다음은 해당 프로젝트에서 고려해야할 사항 원본이다.<br/>

>1.	Object<br/>
The Socket interface is API to bridge between applications and the kernel. The socket programming is useful when an application deals with communicating another process in the same machine or different machines. The socket programming in the IPv4 network is proficient to students.<br/>
>2. Communication protocol<br/>
Students must write socket programming in the client and the server. The student’s client must connect to the designated server running in IPv4 address and 50000 TCP port number: 115.145.170.185(2022fcn.ddns.net). The designated server uses the following protocol to communicate with the client.<br/>
> > 1. First Stage
After connecting to the server, the designated server will ask your ID and your server’s public IP address and port number. Your server should be running at this time and have a valid IPv4 address and TCP port number.
> > 2. Second stage
Our server creates the clients on its machine and opens the second connections to your server on the given the IP address and the port number. Each new client will deliver the 20-byte string (token) on the second connection. Your server must pass all received tokens to your client and your client must return this token to the designated server on the first connection.
The important thing is that you should make your server running concurrently. Our client sends multiple requests from several process at the same time. The number of our clients is five. You should connect all of client processes. After receiving the fifth token, you should combine all tokens to the string and return to our server. Our server will accept the token only if all tokens match exactly. If your program shows the “Result	: Concurrent”, your program is working fine. If your program does not show the any result, it may have a problem in synchronization of our IPv4 server.
The following figure illustrate the protocol. 
![prot](https://user-images.githubusercontent.com/95288696/208396110-8dc3b153-f327-4cd3-bdf7-e7a2401af0e1.svg)
> 3.	Instruction
> > 1.	In the beginning, the banner will welcome you to my machine.
> > ![ins1](https://user-images.githubusercontent.com/95288696/208397608-c369b7ce-5596-43bb-b0d7-6f0071f9ef38.png)
> > 2.	You must return the token received from the second connection to the first connection.
> > 3.	All the input must be terminated by the Line Feed(\n).
> > 4.	You can also check network connection with the designated server using “nc -vz 2022fcn.ddns.net 50000”.
> > 5.	Report immediately if you find a bug.
> > 6.	Make sure that I am available to connect your server.
> > 7.	The screenshot of success concurrent communication
> > ![ins2](https://user-images.githubusercontent.com/95288696/208397349-8db64f11-7171-4690-b6db-13856e7ac5a7.png)
<hr/>

## 보고서

### 구현방법
**Ipv4 client <-> ipv4 server (115.145.170.185)**

먼저 ipv4 client(나의 컴퓨터)에서 학교 서버 115.145.170.185 에 접근하기 위해서 client측 소켓 프로그래밍을 구현해야 한다. 당연히 client측이므로, 소켓을 만들고, connect 하는 과정을 거치는 것은 무리가 없이 진행될 수 있었다. 하지만 무리가 있었던 점들은 서버에서 정보를 받아오고, client측에서 서버를 전송하는 부분이었다. 먼저 ‘buf’ 배열을 두고, read함수를 ‘buf’의 길이만큼 실행시켰다. 하지만 자꾸 개행이 일어났다. 이는 결국 패킷을 받는 수에 문제가 있었다. 그래서 알맞은 화면 정보를 띄우기 위해서 기본적으로 read를 두 번 시킨 후에, print를 하는 방식을 적용시키면 됐었다. 그리고, 내가 서버로 주는 정보도, 정확히 내가 주는 정보의 크기만큼 줘야 한다. 게다가 보내는 정보 마지막에 LINE FEED를 적용시켜야 했다. 해당 부분은 보내는 정보를 담는 ‘buf’ 배열에서 유효한 정보 길이보다 1만큼 큰 바이트까지 write함수에 포함시켜서 보내는 것으로 해결할 수 있었다. 이렇게 하면, 서버와 client간의 원활한 통신이 가능할 수 있었다.<br/>

**내가 만든 IPv4 서버 구현**

정말 어려웠던 부분이었다. 처음에는 포트포워딩이 적용이 안되어서 서버가 학교측 클라이언트들로부터 정보를 못 받나 오해했었다. 이는 내가 서버 IP를 잘못 설정한 것부터 시작이었다. 당연히 공인 IP주소로 서버 IP를 넘겼어야 하는데, 사설 IP주소를 넘겼다. 포트포워딩이 적용되어서 공인 IP로 바뀌는 것이므로 공인IP주소를 입력해야만 한다. 먼저, 서버를 구현하는 데에 있어서 concurrent server를 만들지 못했음을 먼저 말한다. 나의 서버 포트주소는 학번의 뒷 다섯자리인 ‘13665’로 적용시켰다. 서버의 socket programming 형식을 그대로 따라서, 소켓을 만들고, bind하고 listen상태에 접어들게 하였다. Listen 상태에서 서버쪽 클라이언트들이 나의 서버와 통신을 하면 while문을 돈다. 해당 while문에서 accept를 하여 client를 맞이할 준비를 한다. Socket을 잘 생성한 경우에는 각 client와 통신하기 위해서 fork( ) 함수를 실행, 멀티프로세스 방식을 적용하였다. Child process들이 각 client마다 하나씩 배정되면서 해당 클라이언트들이 전송한 정보들을 read 로 ‘buf’배열에 저장하고, 이를 write를 통해서 해당 토큰정보들을 잘 받았는지 확인하는 과정들을 거쳤다. 그리고 정보를 잘 받았으면, 해당 소켓은 종료시킨다. 이때 여기서, ipv4 client (나) 와 통신을 진행해야 한다. 서버 client들이 전송한 token 을 내가 만든 서버에 전송하고, 서버 client들을 위한 fork 된 process에서 다시 ipv4 client인 나에게 token을 전송하는 방식으로 IPv4 Server(내가 만든) 와 IPv4 Client (나)와 전송할 수 있게 하였다. 

**Client가 만든 IPv4 server (13665) <-> IPv4 Client( 13666 ) (나)**

먼저, IPv4 client 쪽을 위한 client.c 를 보자. 본 프로젝트의 마지막 과정은 IPv4 client 에서 IPv4 Server(115.~ ) 으로 token 정보를 그대로 전달해야 한다. 그리고, 이 정보는 fork 프로세스에서 나에게 전달하는 방식으로 구현했다. 이를 위해서, IPv4 client인 나의 쪽도 server로 소켓프로그래밍을 해야 했다. 그래야만 fork 된 프로세스에서 보낸 정보를 listen 중에 받아서 정보를 바로바로 client(나)의 쪽에 저장할 수 있기 때문이다. 따라서, 해당 과정을 진행해야 하는 부분부터, 서버 소켓프로그래밍을 진행하였다. 이 때, 서버를 만든 프로세스는 지금 진행하고 있는 상위 프로세스가 아니라, fork를 시켰다. 왜냐하면, 상위 프로세스는 이미 IPv4(115 .~)와 연결이 되어 있는 상태이므로, 굳이 소켓을 초기화해가며 하는 것은 번거롭기 때문이다. 따라서 fork를 시켜서 fork된 자식 프로세스에서 서버 socket programming을 하였다. 이 프로세스의 포트 번호는 ‘13666’ 으로 정하였다. 그리고서, 내가 만든 IPv4 에서 fork된 프로세스들은 포트번호 ‘13666’인 나의 서버와 통신을 하기 위한 socket programming 을 해야만 한다. 그렇게 하고, 포트번호 ‘13666’번에 보내면, 해당 서버 (port num 13666) 에서는 해당정보들을 pipe에 저장한다. 파이프는 자식 프로세스와 부모 프로세스의 정보 공유를 위한 file descriptor 이다. 이를 활용하여 5개의 process로부터 온 정보들을 pipe에 적용 시키고, pid>0 인 부모프로세스 부분에서 해당 pipe의 정보를 꺼내 누적해서 ‘lastbuf’ 라는 token 정보를 모두 모은 배열에 저장한다. 이 때, 정확히 일치하는 token 값을 얻어내기 위해 자잘한 수정을 하면서 모든 token 정보들을 ‘lastbuf’에 저장시켰다. 또한 자식프로세스와 부모프로세스의 시작 / 종료 순서를 반드시 자식 프로세스를 먼저 실행시켜야 하므로, waitPid 및 wait함수를 이용하였다.

**마지막 토큰 전송**

토큰 5개를 다 받아오면, IPv4 Client 측 부모프로세스 (상위 프로세스)는 이미 IPv4 Server(115.~)과 연결이 되어있으므로, 해당 server로 정보를 전달하면서 끝이 난다.





## < Serialized tokens that you have received from the server >
 
맨 밑줄을 보면 토큰 5개를 잘 받았음을 알 수 있다. 이를 그대로 서버에 보내 보았지만 응답이 오지는 않았다. 패킷을 확인해 봐도 마지막 LINE FEED까지 잘 처리 했음에도 응답이 없었다. 아무래도 순서가 어긋나지 않았나 싶다.
60488990930408350506,
00631383692255864053,
33806271907571276302,
30147295119741421596,
23790131677935652470

## < Special Experience >

-	포트포워딩

포트포워딩에서 정말 많은 경험을 했다. 처음에 시도했던 방법은, 집에 보이는 공유기의 정보로 포트포워딩을 하면 되겠구나 하고 생각을 했었다. 하지만 이는 큰 오해였다. 해당 공유기는 그저 DHCP 로 사설주소를 뿌리는 것이었고, 해당 공유기도 사설 주소였다. 그래서 포트포워딩을 하려고 아무리 공유기랑 씨름을 해도 진전이 나지 않았다. 결국 알게 된 점은 본가에는 공유기가 2개였고, 외부로 나가는 공유기는 집안에 박혀 있었다. 해당 공유기를 통해서 포트포워딩을 하려고, 홈페이지에 들어가서 포트포워딩 정보를 등록하려고 했지만, 아이디와 비밀번호가 일치하지 않았다. 결국 이것도 공유기를 RESET하는 과정에서 해결할 수 있었다. 포트포워딩을 하는 것만으로 해결할 수 있었던 것은 아니다. LINUX를 가상 머신 VM에서 구동시켰는데, 가상머신 ip는 또 컴퓨터 host IP와 다르므로, 가상머신에서도 포트포워딩을 해야하고, 호스트 pc IP도 포트포워딩을 적용시켜야 함을 알게 되었다. 
-	멀티프로세싱

이 기회에 멀티프로세싱을 처음 해보았다. 다음학기에 시스템프로그래밍 실습을 들을 예정이어서 꽤나 많은 도움을 받은 것 같다. 해당과목을 들었다면 시간 내에 프로젝트를 끝냈지 않았을 까 하는 아쉬움이 있다. Fork를 하면 어떻게 되는지, 좀비프로세스는 어떻게 처리하는지, 그리고 signal handler에 대한 과정을 찾아보며 많이 공부하게 되었다. 공부했던 모든 것들을 프로젝트에 녹아낼 수 없어서 아쉬웠다.
또한 보통 프로세스의 공유자원이라고 하면 shared memory, RPC, message queue 정도만 알고 있었는데, pipe가 있다는 것도 알게 되었다.

## 결과 screen shot

![mine](https://user-images.githubusercontent.com/95288696/208418991-f1160193-ea9a-4739-84ab-5a7156a3fe5f.png)

위 사진은 내가 받은 토큰들을 커맨드 창에서 확인한 모습이다.<br/>
이를 실어서 해당 서버에 그대로 보냈지만, 결과 메세지가 도착하지 않았다. 아무래도 순서가 문제인 것 같다.

![mine2](https://user-images.githubusercontent.com/95288696/208419963-81e8f974-46c9-4f1d-a495-2292188d7d48.png)

이 사진은 내가 랩서버에 보낸 패킷이다. 성공적으로 토큰정보가 line feed까지 포함해서 간 것을 볼 수 있다.




 
