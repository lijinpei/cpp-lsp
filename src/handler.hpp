#pragma once

/* This file defines MessageHandler class.
 */

namespace cpplsp {
int handle_request(context &, request_reader & rr, response_writer & rw);
}
