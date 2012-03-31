#ifndef __ERROR_CODE_LIST__
#define __ERROR_CODE_LIST__

#define EXIT_SYSTEM_ERROR				199		// �V�X�e���G���[

#define EXIT_BAD_ARGUMENTS				200	// ����������
#define EXIT_NO_ARGUMENTS				201	// ����������Ȃ�

#define EXIT_SOCKET_ERROR				210	// �\�P�b�g�G���[
#define EXIT_SOCKET_CONNECT_ERROR		211	// ���M�p�\�P�b�g�̍쐬�Ɏ��s
#define EXIT_SOCKET_BIND_LISTEN_ERROR	212	// ��M�p�\�P�b�g�̍쐬�Ɏ��s

#define EXIT_FILE_NOT_FOUND				220	// �t�@�C����������Ȃ�
#define EXIT_INVALID_FILE_CONFIGURATION	221	// �ݒ�t�@�C���̌��

#define EXIT_DEVICE_NOT_FOUND			230	// �f�o�C�X��������Ȃ�
#define EXIT_GAMEPAD_NOT_FOUND			231	// Gamepad��������Ȃ�
#define EXIT_GAMEPAD_SETUP_ERROR		232	// Gamepad�̃Z�b�g�A�b�v�G���[

#define EXIT_RTT4EC_CONNECT_ERROR		240	// RTT���N������Ă��Ȃ����AExternal Connection���g�p�ł��Ȃ�

#define EXIT_NOT_EXECUTABLE				998	// �����`���[����N�������Ƃ��ɕt�^�����N���I�v�V�������Ȃ�
#define EXIT_SOME_ERROR					999	// ��L�ȊO
#endif /* __ERROR_CODE_LIST__ */