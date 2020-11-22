#ifndef DIST_EXCEPTION_H
#define DIST_EXCEPTION_H

namespace dist {

class exception : public std::exception {
  exception(const char *message);
  const char *const what();

private:
  const char *_what;
}

} // namespace dist

#endif