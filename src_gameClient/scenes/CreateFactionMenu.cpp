
#include "CreateFactionMenu.h"

#include "../net/Client.h"

#include "../net/ByteBuffer.h"
#include "../net/Client.h"
#include "../net/requests/platform/web/WebRequest.h"
#include "../net/NetCommon.h"

#include "../../pk/core/Debug.h"


using namespace pk;
using namespace ui;
using namespace net;
using namespace net::web;


CreateFactionMenu::CreateFactionMenu()
{}


CreateFactionMenu::~CreateFactionMenu()
{
	delete _inputField_username;
	delete _inputField_password;
	delete _button_create;
	delete _button_cancel;
}


class OnCompletion_test : public OnCompletionEvent
{
public:

	virtual void func(const std::vector<ByteBuffer>& data)
	{
		Debug::log("Req completed!");

		if (data.size() > 0)
		{
			std::string response(data[0].getRawData(), data[0].getSize());
			Debug::log("Server response: " + response);
		}
	}

};

void CreateFactionMenu::init()
{

	// JUST FOR TESTING
	
	std::string userID_str = "Persekorva123";
	char userID[32];
	memcpy(userID, userID_str.data(), userID_str.size());

	const int func_createFaction = 420;
	int32_t funcName = func_createFaction;

	std::string args = "SuperNation420";

	ByteBuffer buf_userID(userID, 32);;
	ByteBuffer buf_fName(funcName);
	ByteBuffer buf_args(args.data(), args.size());

	
	std::string userID_str2 = "TestUser";
	char userID2[32];
	memcpy(userID2, userID_str2.data(), userID_str2.size());

	std::string args2 = "AmazingFaction";

	ByteBuffer buf_userID2(userID2, 32);;
	ByteBuffer buf_fName2(funcName);
	ByteBuffer buf_args2(args2.data(), args2.size());

	new WebRequest(Request::ReqType::POST, new OnCompletion_test, { buf_userID, buf_fName, buf_args }, buf_userID.getSize() + buf_fName.getSize() + buf_args.getSize());
	new WebRequest(Request::ReqType::POST, new OnCompletion_test, { buf_userID2, buf_fName2, buf_args2 }, buf_userID2.getSize() + buf_fName2.getSize() + buf_args2.getSize());


	const float textSize = 16;
	const float rowPadding = 5;

	const float buttonSize = 24;

	const float panelX = 128;
	const float panelY = 128;

	_inputField_username = new InputField(
		"Enter username",
		{
			{ConstraintType::PIXEL_LEFT, panelX},
			{ConstraintType::PIXEL_TOP, panelY}
		},
		200,
		nullptr
	);

	_inputField_password = new InputField(
		"Enter password",
		{
			{ConstraintType::PIXEL_LEFT, panelX},
			{ConstraintType::PIXEL_TOP, panelY + textSize + rowPadding}
		},
		200,
		nullptr
	);

	_inputField_passwordRepeat = new InputField(
		"Repeat password",
		{
			{ConstraintType::PIXEL_LEFT, panelX},
			{ConstraintType::PIXEL_TOP, panelY + (textSize + rowPadding) * 2}
		},
		200,
		nullptr
	);

	_button_create = new Button(
		"Create user",
		{
			{ConstraintType::PIXEL_LEFT, panelX + 110},
			{ConstraintType::PIXEL_TOP, panelY + (textSize + rowPadding) * 3}
		},
		100,
		buttonSize,
		nullptr
	);

	_button_cancel = new Button(
		"Cancel",
		{
			{ConstraintType::PIXEL_LEFT, panelX},
			{ConstraintType::PIXEL_TOP, panelY + (textSize + rowPadding) * 3}
		},
		100,
		buttonSize,
		nullptr
	);
}

void CreateFactionMenu::update()
{
}