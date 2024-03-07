#ifndef PARSER_TRANSPORT_HPP
#define PARSER_TRANSPORT_HPP

#include "transport.hpp"

#include <boost/asio/ip/tcp.hpp>

namespace ndn {

namespace detail {

template<typename BaseTransport, typename Protocol>
class StreamTransportImpl;

template<typename BaseTransport, typename Protocol>
class StreamTransportWithResolverImpl;

} // namespace detail

/**
 * \brief A transport that uses a TCP socket for communication.
 */
class ParserTransport : public Transport
{
public:
  explicit
  ParserTransport(const std::string& host, const std::string& port = "6363");

  ~ParserTransport() override;

  void
  connect(boost::asio::io_context& ioCtx, ReceiveCallback receiveCallback) override;

  void
  bind(boost::asio::io_context& ioCtx, ReceiveCallback receiveCallback);

  void
  close() override;

  void
  pause() override;

  void
  resume() override;

  void
  send(const Block& wire) override;

  /**
   * \brief Create transport with parameters defined in URI.
   * \throw Transport::Error incorrect URI or unsupported protocol is specified
   */
  static shared_ptr<ParserTransport>
  create(const std::string& uri);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static std::pair<std::string, std::string>
  getSocketHostAndPortFromUri(const std::string& uri);

private:
  std::string m_host;
  std::string m_port;

  using Impl = detail::StreamTransportWithResolverImpl<ParserTransport, boost::asio::ip::tcp>;
  friend class detail::StreamTransportImpl<ParserTransport, boost::asio::ip::tcp>;
  friend class Impl;
  shared_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // NDN_CXX_TRANSPORT_TCP_TRANSPORT_HPP
