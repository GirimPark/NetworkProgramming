// ��ǻ��, ���α׷����� ����ü ���� ��Ģ�� �ٸ� �� �ִ�.
// ����ü�� �����ϰ� #pragma pack�� ���� ���ϴ´�� ������ �� �ִ�.

// ��Ŷ�� ũ�� �ʰ� ���� ũ��ε� ����� ���� ���Ƽ�
// ��Ŷ ���Ǹ� ����ü�� �ϰ� 1����Ʈ ��ŷ�� �ϴ� ��찡 ����.
// ������ ������ ����.
// ������ ��� ������ Ÿ���� �������� �ʴ�.
// �� �� �����Ӱ� ����ϱ� ���� �ø�������� ��Ŷ ����� ����ų� ���� ���̺귯���� ����Ѵ�.
// ������ ���̺귯���δ� FlatBuffer�� �ִ�.
#pragma once

#pragma pack (push, 1)

enum EPacketId
{
	C2S_HELLO = 1,
	S2C_HELLO = 2
};

struct PacketHeader
{
	short size;
	short id;
};

struct PacketC2S_Hello : PacketHeader
{
	char helloKey[100];
};

struct PacketS2C_Welcome : PacketHeader
{
	char welcomeKey[100];
};

#pragma pack (pop)