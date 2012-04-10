#ifndef __SHARED_CONSTANTS__
#define __SHARED_CONSTANTS__

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

#define EXIT_RTTEC_CONNECT_ERROR		240	// RTT���N������Ă��Ȃ����AExternal Connection���g�p�ł��Ȃ�
#define EXIT_CORE_CONNECT_ERROR			250	// I4C3D���N������Ă��Ȃ�

#define EXIT_CERT_UNINITIALIZED			260	// ���C�Z���X�`�F�b�N���s���Ă��Ȃ�
#define EXIT_CERT_INVALID_MACADDRESS	261	// MAC�A�h���X������
#define EXIT_CERT_INVALID_EXPIRE_DATE	262	// �L������������
#define EXIT_CERT_FILE_NOT_FOUND		263	// ���C�Z���X�t�@�C����������Ȃ�
#define EXIT_CERT_SYSTEM_ERROR			264	// ���C�Z���X�t�@�C���`�F�b�N���ɃV�X�e���G���[������

#define EXIT_NOT_EXECUTABLE				998	// �����`���[����N�������Ƃ��ɕt�^�����N���I�v�V�������Ȃ�
#define EXIT_SOME_ERROR					999	// ��L�ȊO


/* Menu Items */
#define MENU_EDIT						"�ݒ�t�@�C����ҏW"
#define MENU_RELOAD						"�����[�h"
#define MENU_END						"�I��"

/* Debug Messages */
#define MESSAGE_DEBUG_LOG_OPEN			"���O�t�@�C���ւ̏������݂��J�n���܂��B"
#define MESSAGE_DEBUG_LOG_CLOSE			"���O�t�@�C���ւ̏������݂��I�����܂��B"

#define MESSAGE_DEBUG_HANDLE_VALID		"���\�[�X�n���h���̏����͐���ɓ��삵�܂����B"

#define MESSAGE_DEBUG_PROCESSING		"���̈ʒu�̃R�[�h�͏������s���܂����B"

/* Info Messages */
#define MESSAGE_INFO_PLUGIN_ALT			"�C���L�[���w�肳��Ȃ��������߁AAlt�L�[�ɐݒ肳��܂����B"

/* Error Messages */
#define MESSAGE_ERROR_XML_LOAD			"XML�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B�ݒ�t�@�C���𐳂����z�u���Ă��������B"
#define MESSAGE_ERROR_XML_TAG_GLOBAL	"XML�t�@�C����global�^�O�̓ǂݍ��݂Ɏ��s���܂����BXML�̍\�������������m�F���Ă��������B"
#define MESSAGE_ERROR_XML_TAG_SOFTS		"XML�t�@�C����softs�^�O�̓ǂݍ��݂Ɏ��s���܂����BXML�̍\�������������m�F���Ă��������B"
#define MESSAGE_ERROR_XML_TAG_SOFTS_DOM	"XML�t�@�C����softs�^�O��DOM�擾�Ɏ��s���܂����BXML�̍\�������������m�F���Ă��������B"

#define MESSAGE_ERROR_SYSTEM_INIT		"�V�X�e���̏������Ɏ��s���܂����B�V�X�e���̐ݒ肪���������m�F���Ă��������B"
#define MESSAGE_ERROR_FUNCTION_PARAM	"�����Ɍ�肪����܂��B"

#define MESSAGE_ERROR_CFG_COREAPPNAME	"�ݒ�t�@�C����core_app_name�^�O�ɃR�A�A�v���P�[�V���������L�ڂ��Ă��������B"
#define MESSAGE_ERROR_CFG_TERMCHAR		"�ݒ�t�@�C���̏I�[�����̎w�肪�Ԉ���Ă��܂��B'?' ��ݒ肵�܂��B"
#define MESSAGE_ERROR_CFG_PORT			"�ݒ�t�@�C���̃|�[�g�ԍ��̎w�肪�����ł��B�|�[�g�ԍ��̐ݒ肪���������m�F���Ă��������B"
#define MESSAGE_ERROR_CFG_MODIFY		"�ݒ�t�@�C���̏C���L�[��K�؂ɐݒ肵�Ă��������B"

#define MESSAGE_ERROR_PLUGIN_INIT		"�v���O�C���̏������Ɏ��s���܂����B�v���O�C���̐ݒ肪���������m�F���Ă��������B"
#define MESSAGE_ERROR_PLUGIN_MACRO		"�v���O�C���̃}�N���̓o�^�Ɏ��s���܂����B"

#define MESSAGE_ERROR_PLUGIN_ARGUMENT	"�����������ł��B���̃A�v���P�[�V�����̓����`���[����N�������K�v������܂��B"
#define MESSAGE_ERROR_PLUGIN_OPTION		"�N���I�v�V�����������ł��B���̃A�v���P�[�V�����̓����`���[����N�������K�v������܂��B"

#define MESSAGE_ERROR_CURSOR_MISSING	"�����J�[�\�������[�h�ł��Ȃ����߁A�J�[�\���̕ύX�͍s���܂���B�����J�[�\���t�@�C�����m�F���Ă��������B"
#define MESSAGE_ERROR_CURSOR_CHANGE		"�����J�[�\���ւ̕ύX�Ɏ��s���܂����B�����J�[�\���t�@�C�����m�F���Ă��������B"

#define MESSAGE_ERROR_SOCKET_INVALID	"�\�P�b�g�̏������Ɏ��s���܂����B�l�b�g���[�N�̐ݒ���m�F���Ă��������B"
#define MESSAGE_ERROR_SOCKET_BIND		"�\�P�b�g�̃o�C���h�Ɏ��s���܂����BOS���ċN�����Ă��������B"
#define MESSAGE_ERROR_SOCKET_LISTEN		"�\�P�b�g�̃��b�X���Ɏ��s���܂����BOS���ċN�����Ă��������B"
#define MESSAGE_ERROR_SOCKET_EVENT		"�\�P�b�g�̃C�x���g�ݒ菈�������s���܂����B�l�b�g���[�N�̐ݒ���m�F���Ă��������B"
#define MESSAGE_ERROR_SOCKET_CONNECT	"�\�P�b�g�̐ڑ����������s���܂����B�l�b�g���[�N�̐ݒ���m�F���Ă��������B"
#define MESSAGE_ERROR_SOCKET_SEND		"�\�P�b�g�̑��M���������s���܂����B"
#define MESSAGE_ERROR_SOCKET_RECV		"�\�P�b�g�̎�M���������s���܂����B"


#define MESSAGE_ERROR_MESSAGE_INVALID	"��M���b�Z�[�W�̉�͂Ɏ��s���܂����B"

#define MESSAGE_ERROR_WINDOW_MISSING	"�C�x���g����͂���E�B���h�E���擾�ł��܂���ł����B"

#define MESSAGE_ERROR_HANDLE_INVALID	"���\�[�X�n���h���̏����Ɏ��s���܂����B�Ǘ��҂ɘA�����Ă��������B"
#define MESSAGE_ERROR_MEMORY_INVALID	"�������̎擾�Ɏ��s���܂����B�Ǘ��҂ɘA�����Ă��������B"

#define MESSAGE_ERROR_DI8_INVALID		"DirectInput�̏������Ɏ��s���܂����BDirectX�̃o�[�W�������m�F���Ă��������B�f�o�C�X���L���ȏ�Ԃ��m�F���Ă��������B"
#define MESSAGE_ERROR_DI8_UNKNOWN		"DirectInput�̏������ɃG���[���������܂����BDirectX�̃o�[�W�������m�F���Ă��������B�f�o�C�X���L���ȏ�Ԃ��m�F���Ă��������B"

#define MESSAGE_ERROR_CERT_FAILED		"���C�Z���X�F�؂Ɏ��s���܂����B"

#define MESSAGE_ERROR_UNKNOWN			"�\�����ʃG���[�������B"

#endif /* __SHARED_CONSTANTS__ */