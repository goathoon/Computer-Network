#include <stdio.h>
#include <stdlib.h>

typedef struct __timeval
{
    unsigned long _sec;
    unsigned long _usec;
} _timeval;

typedef struct __pkthdr
{
    _timeval timeval;
    long caplen;
    long len;
} _pkthdr;

typedef struct __2ndlayer
{
    unsigned char dest_mac[6];
    unsigned char src_mac[6];
    unsigned short type;
} _2ndlayer;

typedef struct __3rdlayer
{
    unsigned char verNhlen;
    unsigned char ToS;
    unsigned short TotalLength;
    unsigned short identification;
    unsigned short fragmentInfo;
    unsigned char TTL;
    unsigned char protocol_id;
    unsigned short checksum;
    unsigned char src_ip[4];
    unsigned char dest_ip[4];
} _3rdlayer;

// Endian 정렬
unsigned short EndianAlign(unsigned short number);
//초단위 UNIX 시간 바꾸는 함수
void PrintTime(unsigned long second, unsigned long microsec);
// captured length, actual length 프린트하는 함수
void PrintTime(unsigned long second, unsigned long microsec)
{
    unsigned long hour = second % 86400 + 3600 * 9;
    unsigned long minute = hour % 3600;
    unsigned long sec = minute % 60;
    printf("Recored Time Of Packet : %02lu:%02lu:%02lu.%06lu\n", (hour / 3600)%24, minute / 60, sec, microsec);
}
// 2계층 정보 프린트하는 함수
void PrintSecond(_2ndlayer layer);
// 3계층 정보 프린트하는 함수
void PrintThird(_3rdlayer layer);

unsigned short EndianAlign(unsigned short number)
{
    return (number >> 8) | (number << 8);
}

void PrintSecond(_2ndlayer layer)
{
    printf("Source Mac Address:");
    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
        {
            printf("%02X", layer.src_mac[i]);
            break;
        }
        printf("%02X:", layer.src_mac[i]);
    }
    printf(" -> ");
    printf("Destination Mac Address:");
    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
        {
            printf("%02X\n", layer.dest_mac[i]);
            break;
        }
        printf("%02X:", layer.dest_mac[i]);
    }
}

void PrintThird(_3rdlayer layer)
{
    printf("Source IP Address:");
    for (int i = 0; i < 4; i++)
    {
        if (i == 3)
        {
            printf("%d", layer.src_ip[i]);
            break;
        }
        printf("%d.", layer.src_ip[i]);
    }
    printf(" -> ");
    printf("Destination IP Address:");
    for (int i = 0; i < 4; i++)
    {
        if (i == 3)
        {
            printf("%d\n", layer.dest_ip[i]);
            break;
        }
        printf("%d.", layer.dest_ip[i]);
    }
    //프로토콜 정보 출력
    printf("Protocol is ");
    switch (layer.protocol_id)
    {
    case 1:
        printf("ICMP\n");
        break;
    case 6:
        printf("TCP\n");
        break;
    case 17:
        printf("UDP\n");
        break;
    default:
        printf("not yet set (out of our format)");
    }

    // fragment 정보 출력
    printf("Identification is %d", EndianAlign(layer.identification));
    unsigned char flags = 0;
    layer.fragmentInfo = EndianAlign(layer.fragmentInfo);
    flags = layer.fragmentInfo >> 13;
    if ((flags - flags % 2) >> 1 == 1)
    {
        printf(" DF:%d\n", 1);
    }
    else
    {
        printf(" MF:%d\n", flags % 2);
    }

    // TTL
    printf("TTL:%d\n", layer.TTL);
    // TOS
    printf("Type Of Service: ");
    if ((layer.ToS >> 2) == 0)
    {
        printf("It is default DSCP:CS0 ");
    }
    else if ((layer.ToS >> 2) == 8)
    {
        printf("DSCP:CS1 ");
    }
    else if ((layer.ToS >> 2) == 16)
    {
        printf("DSCP:CS2 ");
    }
    else if ((layer.ToS >> 2) == 24)
    {
        printf("DSCP:CS3 ");
    }
    else if ((layer.ToS >> 2) == 32)
    {
        printf("DSCP:CS4 ");
    }
    else if ((layer.ToS >> 2) == 40)
    {
        printf("DSCP:CS5 ");
    }
    else if ((layer.ToS >> 2) == 48)
    {
        printf("DSCP:CS6 ");
    }
    else if ((layer.ToS >> 2) == 56)
    {
        printf("DSCP:CS7 ");
    }
    else
    {
        printf("DSCP:Unknown ");
    }
    if (layer.ToS % 4 == 0)
    {
        printf("and Not ECN-Capable Transport\n");
    }
    else if (layer.ToS % 4 == 1 || layer.ToS % 4 == 2)
    {
        printf("and ECN Capable Transport, ECT(1)\n");
    }
    else if (layer.ToS % 4 == 2)
    {
        printf("and ECN Capable Transport, ECT(0)");
    }
    else
    {
        printf("and Congestion Encountered, CE\n");
    }
    printf("---------------------next packet-------------------\n");
}

int main()
{
    _pkthdr pkthdr;
    _2ndlayer second_layer;
    _3rdlayer third_layer;
    long stream_pointer = 24;

    FILE *pcap_file = fopen("filtering.pcap", "rb");
    if (pcap_file == NULL)
    {
        printf("no such file exit.");
        return 0;
    }
    int count = 0;
    fseek(pcap_file, 24, SEEK_SET);
    while (1)
    {
        fread(&pkthdr, sizeof(_pkthdr), 1, pcap_file);
        if (count == 3)
        {
            printf("================= DONE ==================");
            break;
        }
        PrintTime(pkthdr.timeval._sec, pkthdr.timeval._usec);
        printf("captured length: %ld , actual length: %ld, ", pkthdr.caplen, pkthdr.len);
        fread(&second_layer, sizeof(_2ndlayer), 1, pcap_file);
        fread(&third_layer, sizeof(_3rdlayer), 1, pcap_file);
        printf("length of IP header: %d\n", third_layer.verNhlen % 16);
        PrintSecond(second_layer);
        PrintThird(third_layer);

        //알맞은 stream 포인터 위치 조정 (해당 패킷의 len만큼 움직여야한다.)
        stream_pointer += pkthdr.caplen + 16; //해당 패킷의 헤더길이를 포함시켜야함.
        fseek(pcap_file, stream_pointer, SEEK_SET);
        count++;
    }
    fclose(pcap_file);
}
