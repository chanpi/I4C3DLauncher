#ifndef __ERROR_CODE_LIST__
#define __ERROR_CODE_LIST__

#define EXIT_SYSTEM_ERROR				199		// システムエラー

#define EXIT_BAD_ARGUMENTS				200	// 引数が無効
#define EXIT_NO_ARGUMENTS				201	// 引数が足りない

#define EXIT_SOCKET_ERROR				210	// ソケットエラー
#define EXIT_SOCKET_CONNECT_ERROR		211	// 送信用ソケットの作成に失敗
#define EXIT_SOCKET_BIND_LISTEN_ERROR	212	// 受信用ソケットの作成に失敗

#define EXIT_FILE_NOT_FOUND				220	// ファイルが見つからない
#define EXIT_INVALID_FILE_CONFIGURATION	221	// 設定ファイルの誤り

#define EXIT_DEVICE_NOT_FOUND			230	// デバイスが見つからない
#define EXIT_GAMEPAD_NOT_FOUND			231	// Gamepadが見つからない
#define EXIT_GAMEPAD_SETUP_ERROR		232	// Gamepadのセットアップエラー

#define EXIT_RTT4EC_CONNECT_ERROR		240	// RTTが起動されていないか、External Connectionが使用できない

#define EXIT_NOT_EXECUTABLE				998	// ランチャーから起動されるときに付与される起動オプションがない
#define EXIT_SOME_ERROR					999	// 上記以外
#endif /* __ERROR_CODE_LIST__ */