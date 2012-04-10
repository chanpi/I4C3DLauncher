#ifndef __SHARED_CONSTANTS__
#define __SHARED_CONSTANTS__

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

#define EXIT_RTTEC_CONNECT_ERROR		240	// RTTが起動されていないか、External Connectionが使用できない
#define EXIT_CORE_CONNECT_ERROR			250	// I4C3Dが起動されていない

#define EXIT_CERT_UNINITIALIZED			260	// ライセンスチェックが行われていない
#define EXIT_CERT_INVALID_MACADDRESS	261	// MACアドレスが無効
#define EXIT_CERT_INVALID_EXPIRE_DATE	262	// 有効期限が無効
#define EXIT_CERT_FILE_NOT_FOUND		263	// ライセンスファイルが見つからない
#define EXIT_CERT_SYSTEM_ERROR			264	// ライセンスファイルチェック中にシステムエラーが発生

#define EXIT_NOT_EXECUTABLE				998	// ランチャーから起動されるときに付与される起動オプションがない
#define EXIT_SOME_ERROR					999	// 上記以外


/* Menu Items */
#define MENU_EDIT						"設定ファイルを編集"
#define MENU_RELOAD						"リロード"
#define MENU_END						"終了"

/* Debug Messages */
#define MESSAGE_DEBUG_LOG_OPEN			"ログファイルへの書き込みを開始します。"
#define MESSAGE_DEBUG_LOG_CLOSE			"ログファイルへの書き込みを終了します。"

#define MESSAGE_DEBUG_HANDLE_VALID		"リソースハンドルの処理は正常に動作しました。"

#define MESSAGE_DEBUG_PROCESSING		"この位置のコードは処理が行われました。"

/* Info Messages */
#define MESSAGE_INFO_PLUGIN_ALT			"修飾キーが指定されなかったため、Altキーに設定されました。"

/* Error Messages */
#define MESSAGE_ERROR_XML_LOAD			"XMLファイルの読み込みに失敗しました。設定ファイルを正しく配置してください。"
#define MESSAGE_ERROR_XML_TAG_GLOBAL	"XMLファイルのglobalタグの読み込みに失敗しました。XMLの構文が正しいか確認してください。"
#define MESSAGE_ERROR_XML_TAG_SOFTS		"XMLファイルのsoftsタグの読み込みに失敗しました。XMLの構文が正しいか確認してください。"
#define MESSAGE_ERROR_XML_TAG_SOFTS_DOM	"XMLファイルのsoftsタグのDOM取得に失敗しました。XMLの構文が正しいか確認してください。"

#define MESSAGE_ERROR_SYSTEM_INIT		"システムの初期化に失敗しました。システムの設定が正しいか確認してください。"
#define MESSAGE_ERROR_FUNCTION_PARAM	"引数に誤りがあります。"

#define MESSAGE_ERROR_CFG_COREAPPNAME	"設定ファイルのcore_app_nameタグにコアアプリケーション名を記載してください。"
#define MESSAGE_ERROR_CFG_TERMCHAR		"設定ファイルの終端文字の指定が間違っています。'?' を設定します。"
#define MESSAGE_ERROR_CFG_PORT			"設定ファイルのポート番号の指定が無効です。ポート番号の設定が正しいか確認してください。"
#define MESSAGE_ERROR_CFG_MODIFY		"設定ファイルの修飾キーを適切に設定してください。"

#define MESSAGE_ERROR_PLUGIN_INIT		"プラグインの初期化に失敗しました。プラグインの設定が正しいか確認してください。"
#define MESSAGE_ERROR_PLUGIN_MACRO		"プラグインのマクロの登録に失敗しました。"

#define MESSAGE_ERROR_PLUGIN_ARGUMENT	"引数が無効です。このアプリケーションはランチャーから起動される必要があります。"
#define MESSAGE_ERROR_PLUGIN_OPTION		"起動オプションが無効です。このアプリケーションはランチャーから起動される必要があります。"

#define MESSAGE_ERROR_CURSOR_MISSING	"透明カーソルをロードできないため、カーソルの変更は行われません。透明カーソルファイルを確認してください。"
#define MESSAGE_ERROR_CURSOR_CHANGE		"透明カーソルへの変更に失敗しました。透明カーソルファイルを確認してください。"

#define MESSAGE_ERROR_SOCKET_INVALID	"ソケットの初期化に失敗しました。ネットワークの設定を確認してください。"
#define MESSAGE_ERROR_SOCKET_BIND		"ソケットのバインドに失敗しました。OSを再起動してください。"
#define MESSAGE_ERROR_SOCKET_LISTEN		"ソケットのリッスンに失敗しました。OSを再起動してください。"
#define MESSAGE_ERROR_SOCKET_EVENT		"ソケットのイベント設定処理が失敗しました。ネットワークの設定を確認してください。"
#define MESSAGE_ERROR_SOCKET_CONNECT	"ソケットの接続処理が失敗しました。ネットワークの設定を確認してください。"
#define MESSAGE_ERROR_SOCKET_SEND		"ソケットの送信処理が失敗しました。"
#define MESSAGE_ERROR_SOCKET_RECV		"ソケットの受信処理が失敗しました。"


#define MESSAGE_ERROR_MESSAGE_INVALID	"受信メッセージの解析に失敗しました。"

#define MESSAGE_ERROR_WINDOW_MISSING	"イベントを入力するウィンドウが取得できませんでした。"

#define MESSAGE_ERROR_HANDLE_INVALID	"リソースハンドルの処理に失敗しました。管理者に連絡してください。"
#define MESSAGE_ERROR_MEMORY_INVALID	"メモリの取得に失敗しました。管理者に連絡してください。"

#define MESSAGE_ERROR_DI8_INVALID		"DirectInputの初期化に失敗しました。DirectXのバージョンを確認してください。デバイスが有効な状態か確認してください。"
#define MESSAGE_ERROR_DI8_UNKNOWN		"DirectInputの処理中にエラーが発生しました。DirectXのバージョンを確認してください。デバイスが有効な状態か確認してください。"

#define MESSAGE_ERROR_CERT_FAILED		"ライセンス認証に失敗しました。"

#define MESSAGE_ERROR_UNKNOWN			"予期せぬエラーが発生。"

#endif /* __SHARED_CONSTANTS__ */