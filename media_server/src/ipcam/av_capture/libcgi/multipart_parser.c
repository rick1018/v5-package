/* Based on node-formidable by Felix Geisendörfer 
 * Igor Afonov - afonov@gmail.com - 2012
 * MIT License - http://www.opensource.org/licenses/mit-license.php 
 
Usage (C)

This parser library works with several callbacks, which the user may set up at application initialization time.

multipart_parser_settings callbacks;

memset(&callbacks, 0, sizeof(multipart_parser_settings));

callbacks.on_header_field = read_header_name;
callbacks.on_header_value = read_header_value;
These functions must match the signatures defined in the multipart-parser header file. For this simple example, we'll just use two of the available callbacks to print all headers the library finds in multipart messages.

Returning a value other than 0 from the callbacks will abort message processing.

int read_header_name(multipart_parser* p, const char *at, size_t length)
{
   printf("%.*s: ", length, at);
   return 0;
}

int read_header_value(multipart_parser* p, const char *at, size_t length)
{
   printf("%.*s\n", length, at);
   return 0;
}
When a message arrives, callers must parse the multipart boundary from the Content-Type header (see the RFC for more information and examples), and then execute the parser.

multipart_parser* parser = multipart_parser_init(boundary, &callbacks);
multipart_parser_execute(parser, body, length);
multipart_parser_free(parser);

Usage (C++)

In C++, when the callbacks are static member functions it may be helpful to pass the instantiated multipart consumer along as context. The following (abbreviated) class called MultipartConsumer shows how to pass this to callback functions in order to access non-static member data.

class MultipartConsumer
{
public:
    MultipartConsumer(const std::string& boundary)
    {
        memset(&m_callbacks, 0, sizeof(multipart_parser_settings));
        m_callbacks.on_header_field = ReadHeaderName;
        m_callbacks.on_header_value = ReadHeaderValue;

        m_parser = multipart_parser_init(boundary.c_str(), &callbacks);
        multipart_parser_set_data(m_parser, this);
    }

    ~MultipartConsumer()
    {
        multipart_parser_free(m_parser);
    }

    int CountHeaders(const std::string& body)
    {
        multipart_parser_execute(m_parser, body.c_str(), body.size());
        return m_headers;
    }

private:
    static int ReadHeaderName(multipart_parser* p, const char *at, size_t length)
    {
        MultipartConsumer* me = (MultipartConsumer*)multipart_parser_get_data(p);
        me->m_headers++;
    }

    multipart_parser* m_parser;
    multipart_parser_settings m_callbacks;
    int m_headers;
};
*/ 

#include "multipart_parser.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static void multipart_log(const char * format, ...)
{
#ifdef DEBUG_MULTIPART
    va_list args;
    va_start(args, format);

    fprintf(stderr, "[HTTP_MULTIPART_PARSER] %s:%d: ", __FILE__, __LINE__);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
#endif
}

#define NOTIFY_CB(FOR)                                                 \
do {                                                                   \
  if (p->settings->on_##FOR) {                                         \
    if (p->settings->on_##FOR(p) != 0) {                               \
      return i;                                                        \
    }                                                                  \
  }                                                                    \
} while (0)

#define EMIT_DATA_CB(FOR, ptr, len)                                    \
do {                                                                   \
  if (p->settings->on_##FOR) {                                         \
    if (p->settings->on_##FOR(p, ptr, len) != 0) {                     \
      return i;                                                        \
    }                                                                  \
  }                                                                    \
} while (0)


#define LF 10
#define CR 13

struct multipart_parser {
  void * data;

  size_t index;
  size_t boundary_length;

  unsigned char state;

  const multipart_parser_settings* settings;

  char* lookbehind;
  char multipart_boundary[1];
};

enum state {
  s_uninitialized = 1,
  s_start,
  s_start_boundary,
  s_header_field_start,
  s_header_field,
  s_headers_almost_done,
  s_header_value_start,
  s_header_value,
  s_header_value_almost_done,
  s_part_data_start,
  s_part_data,
  s_part_data_almost_boundary,
  s_part_data_boundary,
  s_part_data_almost_end,
  s_part_data_end,
  s_part_data_final_hyphen,
  s_end
};

multipart_parser* multipart_parser_init
    (const char *boundary, const multipart_parser_settings* settings) {

  multipart_parser* p = malloc(sizeof(multipart_parser) +
                               strlen(boundary) +
                               strlen(boundary) + 9);

  strcpy(p->multipart_boundary, boundary);
  p->boundary_length = strlen(boundary);
  
  p->lookbehind = (p->multipart_boundary + p->boundary_length + 1);

  p->index = 0;
  p->state = s_start;
  p->settings = settings;

  return p;
}

void multipart_parser_free(multipart_parser* p) {
  free(p);
}

void multipart_parser_set_data(multipart_parser *p, void *data) {
    p->data = data;
}

void *multipart_parser_get_data(multipart_parser *p) {
    return p->data;
}

size_t multipart_parser_execute(multipart_parser* p, const char *buf, size_t len) {
  size_t i = 0;
  size_t mark = 0;
  char c, cl;
  int is_last = 0;

  while(i < len) {
    c = buf[i];
    is_last = (i == (len - 1));
    switch (p->state) {
      case s_start:
        multipart_log("s_start");
        p->index = 0;
        p->state = s_start_boundary;

      /* fallthrough */
      case s_start_boundary:
        multipart_log("s_start_boundary");
        if (p->index == p->boundary_length) {
          if (c != CR) {
            return i;
          }
          p->index++;
          break;
        } else if (p->index == (p->boundary_length + 1)) {
          if (c != LF) {
            return i;
          }
          p->index = 0;
          NOTIFY_CB(part_data_begin);
          p->state = s_header_field_start;
          break;
        }
        if (c != p->multipart_boundary[p->index]) {
          return i;
        }
        p->index++;
        break;

      case s_header_field_start:
        multipart_log("s_header_field_start");
        mark = i;
        p->state = s_header_field;

      /* fallthrough */
      case s_header_field:
        multipart_log("s_header_field");
        if (c == CR) {
          p->state = s_headers_almost_done;
          break;
        }

        if (c == '-') {
          break;
        }

        if (c == ':') {
          EMIT_DATA_CB(header_field, buf + mark, i - mark);
          p->state = s_header_value_start;
          break;
        }

        cl = tolower(c);
        if (cl < 'a' || cl > 'z') {
          multipart_log("invalid character in header name");
          return i;
        }
        if (is_last)
            EMIT_DATA_CB(header_field, buf + mark, (i - mark) + 1);
        break;

      case s_headers_almost_done:
        multipart_log("s_headers_almost_done");
        if (c != LF) {
          return i;
        }

        p->state = s_part_data_start;
        break;

      case s_header_value_start:
        multipart_log("s_header_value_start");
        if (c == ' ') {
          break;
        }

        mark = i;
        p->state = s_header_value;

      /* fallthrough */
      case s_header_value:
        multipart_log("s_header_value");
        if (c == CR) {
          EMIT_DATA_CB(header_value, buf + mark, i - mark);
          p->state = s_header_value_almost_done;
        }
        if (is_last)
            EMIT_DATA_CB(header_value, buf + mark, (i - mark) + 1);
        break;

      case s_header_value_almost_done:
        multipart_log("s_header_value_almost_done");
        if (c != LF) {
          return i;
        }
        p->state = s_header_field_start;
        break;

      case s_part_data_start:
        multipart_log("s_part_data_start");
        NOTIFY_CB(headers_complete);
        mark = i;
        p->state = s_part_data;

      /* fallthrough */
      case s_part_data:
        multipart_log("s_part_data");
        if (c == CR) {
            EMIT_DATA_CB(part_data, buf + mark, i - mark);
            mark = i;
            p->state = s_part_data_almost_boundary;
            p->lookbehind[0] = CR;
            break;
        }
        if (is_last)
            EMIT_DATA_CB(part_data, buf + mark, (i - mark) + 1);
        break;

      case s_part_data_almost_boundary:
        multipart_log("s_part_data_almost_boundary");
        if (c == LF) {
            p->state = s_part_data_boundary;
            p->lookbehind[1] = LF;
            p->index = 0;
            break;
        }
        EMIT_DATA_CB(part_data, p->lookbehind, 1);
        p->state = s_part_data;
        mark = i --;
        break;

      case s_part_data_boundary:
        multipart_log("s_part_data_boundary");
        if (p->multipart_boundary[p->index] != c) {
          EMIT_DATA_CB(part_data, p->lookbehind, 2 + p->index);
          p->state = s_part_data;
          mark = i --;
          break;
        }
        p->lookbehind[2 + p->index] = c;
        if ((++ p->index) == p->boundary_length) {
            NOTIFY_CB(part_data_end);
            p->state = s_part_data_almost_end;
        }
        break;

      case s_part_data_almost_end:
        multipart_log("s_part_data_almost_end");
        if (c == '-') {
            p->state = s_part_data_final_hyphen;
            break;
        }
        if (c == CR) {
            p->state = s_part_data_end;
            break;
        }
        return i;
   
      case s_part_data_final_hyphen:
        multipart_log("s_part_data_final_hyphen");
        if (c == '-') {
            NOTIFY_CB(body_end);
            p->state = s_end;
            break;
        }
        return i;

      case s_part_data_end:
        multipart_log("s_part_data_end");
        if (c == LF) {
            p->state = s_header_field_start;
            NOTIFY_CB(part_data_begin);
            break;
        }
        return i;

      case s_end:
        multipart_log("s_end: %02X", (int) c);
        break;

      default:
        multipart_log("Multipart parser unrecoverable error");
        return 0;
    }
    ++ i;
  }

  return len;
}
