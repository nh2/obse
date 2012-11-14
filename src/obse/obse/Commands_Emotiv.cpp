#include <stdio.h>
#include <string.h>
#include "Commands_Emotiv.h"
#include "Script.h"
// #include "G:\GitHub\vicarious\Vicarious\c\consumer.h"
#include <consumer.h>

#include <sstream>

template <typename T> std::string ToString(const T& value) {
	std::stringstream os;
	os << value;
	return os.str();
}

// Execute a command and get the results in a string.
// Synchronously launches a child process and waits up to dwMilliseconds for completion.
// Uses a pipe to get the output of the child process.
// Does not pipe to stdin of child process.
// Example usage:
//   std::string str;
//   str = ExecCmd( "C:\\WINDOWS\\System32\\ipconfig.exe", 2000 );
//
std::string ExecCmd(LPCSTR pCmdArg, DWORD dwMilliseconds)
{
	 // Handle Inheritance - to pipe child's stdout via pipes to parent, handles must be inherited.
	 //   SECURITY_ATTRIBUTES.bInheritHandle must be TRUE
	 //   CreateProcess parameter bInheritHandles must be TRUE;
	 //   STARTUPINFO.dwFlags must have STARTF_USESTDHANDLES set.
	 
	 std::string strResult; // Contains result of cmdArg.

	 HANDLE hChildStdoutRd; // Read-side, used in calls to ReadFile() to get child's stdout output.
	 HANDLE hChildStdoutWr; // Write-side, given to child process using si struct.

	 BOOL fSuccess;

	 // Create security attributes to create pipe.
	 SECURITY_ATTRIBUTES saAttr  = { sizeof( SECURITY_ATTRIBUTES ) } ;
	 saAttr.bInheritHandle       = TRUE; // Set the bInheritHandle flag so pipe handles are inherited by child process. Required.
	 saAttr.lpSecurityDescriptor = NULL;

	 // Create a pipe to get results from child's stdout.
	 // I'll create only 1 because I don't need to pipe to the child's stdin.
	 if ( !CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
	 {
			return strResult;
	 }

	 STARTUPINFO si = { sizeof( STARTUPINFO ) }; // specifies startup parameters for child process.

	 si.dwFlags    = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; // STARTF_USESTDHANDLES is Required.
	 si.hStdOutput = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
	 si.hStdError  = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
	 // si.hStdInput remains null.
	 si.wShowWindow = SW_HIDE; // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	 PROCESS_INFORMATION pi  = { 0 };

	 // Create the child process.
	 fSuccess = CreateProcess(
			pCmdArg,            // command line
			NULL,               // arguments
			NULL,               // process security attributes
			NULL,               // primary thread security attributes
			true,               // TRUE=handles are inherited. Required.
			CREATE_NEW_CONSOLE, // creation flags
			NULL,               // use parent's environment
			NULL,               // use parent's current directory
			&si,                // __in, STARTUPINFO pointer
			&pi);               // __out, receives PROCESS_INFORMATION

	 if (! fSuccess)
	 {
		printf( "CreateProcess failed (%d)\n", GetLastError() );
			return strResult;
	 }

	 // Wait until child processes exit. Don't wait forever.
	 WaitForSingleObject( pi.hProcess, dwMilliseconds );
	 TerminateProcess( pi.hProcess, 0 ); // Kill process if it is still running. Tested using cmd "ping blah -n 99"

	 // Close the write end of the pipe before reading from the read end of the pipe.
	 if (!CloseHandle(hChildStdoutWr))
	 {
			return strResult;
	 }

	 // Read output from the child process.
	 for (;;)
	 {
			DWORD dwRead;
			CHAR chBuf[4096];

			// Read from pipe that is the standard output for child process.
			bool done = !ReadFile( hChildStdoutRd, chBuf, 4096, &dwRead, NULL) || dwRead == 0;
			if( done )
			{
				 break;
			}

			// Append result to string.
			strResult += chBuf;
	 }

	 // Close process and thread handles.
	 CloseHandle( hChildStdoutRd );

	 // CreateProcess docs specify that these must be closed. 
	 CloseHandle( pi.hProcess );
	 CloseHandle( pi.hThread );

	 return strResult;
}



//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
#include <string>
#include <iostream>



std::string ZmqGetMessage (std::string outMsg)
{
		//  Prepare our context and socket
		zmq::context_t context (1);
		zmq::socket_t socket (context, ZMQ_REQ);

		socket.connect ("tcp://128.243.22.216:5555");

	zmq::message_t request (outMsg.length ());  // Warning: On Windows, request.data() is not initialized with 0s.

	// Fill request data
	memcpy ((void *) request.data (), outMsg.c_str(), outMsg.length ());  // ZMQ uses length fields, don't set trailing \n.

	// Send request
		socket.send (request);

		//  Get the reply.
		zmq::message_t reply;
		socket.recv (&reply);
		
	std::string data = std::string((const char *) reply.data (), reply.size ());

	return data;
}



#ifdef OBLIVION

#include "GameAPI.h"

/* Run ipconfig.
 * syntax:  RunIpconfig
 * shortname: ipconfig
 */
bool Cmd_RunIpconfig_Execute(COMMAND_ARGS)
{
	Console_Print("Running ipconfig");

	std::string out = ExecCmd("C:\\WINDOWS\\System32\\ipconfig.exe", 2000);
	Console_Print(out.c_str());

	return true;
}

/* Run some Emotiv test.
 * syntax:  EmotivTest some_integer
 * shortname: emotivtest
 */
bool Cmd_EmotivTest_Execute(COMMAND_ARGS)
{
	// Arguments
	int intArg = 0;
	ExtractArgs(PASS_EXTRACT_ARGS, &intArg);

	Console_Print("Requesting message over network");
	std::string outMsg = "Message from Oblivion. Parameter: " + ToString(intArg);

	std::string msg = ZmqGetMessage(outMsg);

	Console_Print("Response:");
	Console_Print(msg.c_str());

	return true;
}


#define CONSUMER_CONNECT_TIMEOUT_MS 200
#define CONSUMER_RECV_TIMEOUT_MS 20

vicarious_consumer_t consumer;

void consumer_error (char * err)
{
	Console_Print(err);
	// TODO check consumer memory free / call destroy
	consumer = NULL;
}


/* TODO doc */
bool Cmd_ConsumerStart_Execute(COMMAND_ARGS)
{
	// Arguments
	int intArg = 0;
	ExtractArgs(PASS_EXTRACT_ARGS, &intArg);

	Console_Print("Starting consumer");

	apr_initialize ();

	const char* host = "llw";
	apr_port_t port = 1234;

	consumer = vicarious_consumer_create (host, port);

	// TODO if (!consumer)
	apr_status_t ret;

	ret = vicarious_consumer_connect (consumer, CONSUMER_CONNECT_TIMEOUT_MS * 1000);

	if (ret != APR_SUCCESS)
	{
	  if (APR_STATUS_IS_INCOMPLETE (ret) || APR_STATUS_IS_BADCH (ret))
	  {
			consumer_error("Consumer: server did not accept query string");
	  }
		consumer_error("Consumer: could not connect");
	}
	else {
		Console_Print("Consumer: connected");
	}

	return true;
}


/* TODO doc */
bool Cmd_ConsumerGet_Execute(COMMAND_ARGS)
{
	if (!consumer)
	{
		Console_Print("Consumer: not connected");
		return false;
	}

	int received_new_value = -1;
	int bytes_received = -1;

	apr_status_t ret;
	ret = vicarious_consumer_recv (consumer,
	                               CONSUMER_RECV_TIMEOUT_MS * 1000,
	                               &received_new_value,
	                               &bytes_received);


	if (APR_STATUS_IS_EOF (ret))
	{
	  /* The consumer freed itself. */
	  consumer_error ("Consumer: end of input");
	  return false;
	}
	else if (APR_STATUS_IS_TIMEUP (ret) || !received_new_value)
	{
		Console_Print("Consumer: no new value");
	}
	else if (ret == APR_SUCCESS)
	{
	  const char *line = vicarious_consumer_get (consumer);

	  if (line != NULL)
	  {
			Console_Print("Consumer: received:");
			Console_Print(line);
	  }
	}
	else
	{
	  consumer_error ("Consumer: vicarious_consumer_recv() failed");
	  return false;
	}

	return true;
}
#endif

static ParamInfo kParams_EmotivTest[] =
{
	{	"int", kParamType_Integer,	0	}
};

static ParamInfo kParams_ConsumerStart[] =
{
	{	"int", kParamType_Integer,	0	}
};

static ParamInfo kParams_ConsumerGet[] =
{
	{	"int", kParamType_Integer,	0	}
};

CommandInfo kCommandInfo_EmotivTest =
{
	"EmotivTest",
	"emo",
	0,
	"Do some Emotiv stuff",
	0,
	1,
	kParams_EmotivTest,
	HANDLER(Cmd_EmotivTest_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_RunIpconfig =
{
	"RunIpconfig",
	"ipconfig",
	0,
	"Executes C:\\WINDOWS\\System32\\ipconfig.exe and prints the result output",
	0,
	0,
	NULL,
	HANDLER(Cmd_RunIpconfig_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_ConsumerStart =
{
	"ConsumerStart",
	"vconstart",
	0,
	"Start the vicarious consumer",
	0,
	1,
	kParams_ConsumerStart,
	HANDLER(Cmd_ConsumerStart_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ConsumerGet =
{
	"ConsumerGet",
	"vconget",
	0,
	"Get recent vicarious consumer value",
	0,
	1,
	kParams_ConsumerGet,
	HANDLER(Cmd_ConsumerGet_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
