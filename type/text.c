#include <someip.h>

char const *
someip_type_text(u1_t type)
{
	switch (type)
	{
	case someip_type_REQUEST:
		return "REQUEST";

	case someip_type_REQUEST_NO_RETURN:
		return "REQUEST_NO_RETURN";

	case someip_type_NOTIFICATION:
		return "NOTIFICATION";

	case someip_type_REQUEST_ACK:
		return "REQUEST_ACK";

	case someip_type_REQUEST_NO_RETURN_ACK:
		return "REQUEST_NO_RETURN_ACK";

	case someip_type_NOTIFICATION_ACK:
		return "NOTIFICATION_ACK";

	case someip_type_RESPONSE:
		return "RESPONSE";

	case someip_type_ERROR:
		return "ERROR";

	case someip_type_RESPONSE_ACK:
		return "RESPONSE_ACK";

	case someip_type_ERROR_ACK:
		return "ERROR_ACK";
	}

	return "unknown";
}
