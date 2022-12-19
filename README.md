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

