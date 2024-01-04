// ��ǻ��, ���α׷����� ����ü ���� ��Ģ�� �ٸ� �� �ִ�.
// ����ü�� �����ϰ� #pragma pack�� ���� ���ϴ´�� ������ �� �ִ�.

// ��Ŷ�� ũ�� �ʰ� ���� ũ��ε� ����� ���� ���Ƽ�
// ��Ŷ ���Ǹ� ����ü�� �ϰ� 1����Ʈ ��ŷ�� �ϴ� ��찡 ����.
// ������ ������ ����.
// ������ ��� ������ Ÿ���� �������� �ʴ�.
// �� �� �����Ӱ� ����ϱ� ���� �ø�������� ��Ŷ ����� ����ų� ���� ���̺귯���� ����Ѵ�.
// ������ ���̺귯���δ� FlatBuffer�� �ִ�.
#pragma once

#include <cstddef>
#pragma pack (push, 1)

enum EPacketId : short
{
	PACKETID_START,

	C2S_ACCESS,
	S2C_ACCESS,
	C2S_BROADCAST_MSG,
	S2C_BROADCAST_MSG,

	PACKETID_END
};

struct PacketHeader
{
	short size;
	EPacketId id;
};

struct PacketC2S_Access : PacketHeader
{
	char* nickname;
};

struct PacketS2C_Access : PacketHeader
{
	std::byte result;
};

struct PacketC2S_BroadcastMsg : PacketHeader
{
	char* clientMessage;
};

struct PacketS2C_BroadcastMsg : PacketHeader
{
	char* serverMessage;
};

#pragma pack (pop)