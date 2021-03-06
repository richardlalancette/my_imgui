//
// stream_socket_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_STREAM_SOCKET_SERVICE_HPP
#define ASIO_STREAM_SOCKET_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "RCF/external/asio/asio/detail/config.hpp"
#include <cstddef>
#include "RCF/external/asio/asio/error.hpp"
#include "RCF/external/asio/asio/io_service.hpp"

#if defined(ASIO_HAS_IOCP)
# include "RCF/external/asio/asio/detail/win_iocp_socket_service.hpp"
#else
# include "RCF/external/asio/asio/detail/reactive_socket_service.hpp"
#endif

#include "RCF/external/asio/asio/detail/push_options.hpp"

namespace asio {

/// Default service implementation for a stream socket.
template <typename Protocol>
class stream_socket_service
#if defined(GENERATING_DOCUMENTATION)
  : public asio::io_service::service
#else
  : public asio::detail::service_base<stream_socket_service<Protocol> >
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static asio::io_service::id id;
#endif

  /// The protocol type.
  typedef Protocol protocol_type;

  /// The endpoint type.
  typedef typename Protocol::endpoint endpoint_type;

private:
  // The type of the platform-specific implementation.
#if defined(ASIO_HAS_IOCP)
  typedef detail::win_iocp_socket_service<Protocol> service_impl_type;
#else
  typedef detail::reactive_socket_service<Protocol> service_impl_type;
#endif

public:
  /// The type of a stream socket implementation.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef typename service_impl_type::implementation_type implementation_type;
#endif

  /// The native socket type.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_type;
#else
  typedef typename service_impl_type::native_type native_type;
#endif

  /// Construct a new stream socket service for the specified io_service.
  explicit stream_socket_service(asio::io_service& io_service)
    : asio::detail::service_base<
        stream_socket_service<Protocol> >(io_service),
      service_impl_(io_service)
  {
  }

  /// Destroy all user-defined handler objects owned by the service.
  void shutdown_service()
  {
    service_impl_.shutdown_service();
  }

  /// Construct a new stream socket implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

  /// Destroy a stream socket implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Open a stream socket.
  asio::error_code open(implementation_type& impl,
      const protocol_type& protocol, asio::error_code& ec)
  {
    if (protocol.type() == SOCK_STREAM)
      service_impl_.open(impl, protocol, ec);
    else
      ec = asio::error::invalid_argument;
    return ec;
  }

  /// Assign an existing native socket to a stream socket.
  asio::error_code assign(implementation_type& impl,
      const protocol_type& protocol, const native_type& native_socket,
      asio::error_code& ec)
  {
    return service_impl_.assign(impl, protocol, native_socket, ec);
  }

  /// Determine whether the socket is open.
  bool is_open(const implementation_type& impl) const
  {
    return service_impl_.is_open(impl);
  }

  /// Close a stream socket implementation.
  asio::error_code close(implementation_type& impl,
      asio::error_code& ec)
  {
    return service_impl_.close(impl, ec);
  }

  /// Get the native socket implementation.
  native_type native(implementation_type& impl)
  {
    return service_impl_.native(impl);
  }

  /// Cancel all asynchronous operations associated with the socket.
  asio::error_code cancel(implementation_type& impl,
      asio::error_code& ec)
  {
    return service_impl_.cancel(impl, ec);
  }

  /// Determine whether the socket is at the out-of-band data mark.
  bool at_mark(const implementation_type& impl,
      asio::error_code& ec) const
  {
    return service_impl_.at_mark(impl, ec);
  }

  /// Determine the number of bytes available for reading.
  std::size_t available(const implementation_type& impl,
      asio::error_code& ec) const
  {
    return service_impl_.available(impl, ec);
  }

  /// Bind the stream socket to the specified local endpoint.
  asio::error_code bind(implementation_type& impl,
      const endpoint_type& endpoint, asio::error_code& ec)
  {
    return service_impl_.bind(impl, endpoint, ec);
  }

  /// Connect the stream socket to the specified endpoint.
  asio::error_code connect(implementation_type& impl,
      const endpoint_type& peer_endpoint, asio::error_code& ec)
  {
    return service_impl_.connect(impl, peer_endpoint, ec);
  }

  /// Start an asynchronous connect.
  template <typename ConnectHandler>
  void async_connect(implementation_type& impl,
      const endpoint_type& peer_endpoint, ConnectHandler handler)
  {
    service_impl_.async_connect(impl, peer_endpoint, handler);
  }

  /// Set a socket option.
  template <typename SettableSocketOption>
  asio::error_code set_option(implementation_type& impl,
      const SettableSocketOption& option, asio::error_code& ec)
  {
    return service_impl_.set_option(impl, option, ec);
  }

  /// Get a socket option.
  template <typename GettableSocketOption>
  asio::error_code get_option(const implementation_type& impl,
      GettableSocketOption& option, asio::error_code& ec) const
  {
    return service_impl_.get_option(impl, option, ec);
  }

  /// Perform an IO control command on the socket.
  template <typename IoControlCommand>
  asio::error_code io_control(implementation_type& impl,
      IoControlCommand& command, asio::error_code& ec)
  {
    return service_impl_.io_control(impl, command, ec);
  }

  /// Get the local endpoint.
  endpoint_type local_endpoint(const implementation_type& impl,
      asio::error_code& ec) const
  {
    return service_impl_.local_endpoint(impl, ec);
  }

  /// Get the remote endpoint.
  endpoint_type remote_endpoint(const implementation_type& impl,
      asio::error_code& ec) const
  {
    return service_impl_.remote_endpoint(impl, ec);
  }

  /// Disable sends or receives on the socket.
  asio::error_code shutdown(implementation_type& impl,
      socket_base::shutdown_type what, asio::error_code& ec)
  {
    return service_impl_.shutdown(impl, what, ec);
  }

  /// Send the given data to the peer.
  template <typename ConstBufferSequence>
  std::size_t send(implementation_type& impl,
      const ConstBufferSequence& buffers,
      socket_base::message_flags flags, asio::error_code& ec)
  {
    return service_impl_.send(impl, buffers, flags, ec);
  }

  /// Start an asynchronous send.
  template <typename ConstBufferSequence, typename WriteHandler>
  void async_send(implementation_type& impl,
      const ConstBufferSequence& buffers,
      socket_base::message_flags flags, WriteHandler handler)
  {
    service_impl_.async_send(impl, buffers, flags, handler);
  }

  /// Receive some data from the peer.
  template <typename MutableBufferSequence>
  std::size_t receive(implementation_type& impl,
      const MutableBufferSequence& buffers,
      socket_base::message_flags flags, asio::error_code& ec)
  {
    return service_impl_.receive(impl, buffers, flags, ec);
  }

  /// Start an asynchronous receive.
  template <typename MutableBufferSequence, typename ReadHandler>
  void async_receive(implementation_type& impl,
      const MutableBufferSequence& buffers,
      socket_base::message_flags flags, ReadHandler handler)
  {
    service_impl_.async_receive(impl, buffers, flags, handler);
  }

private:
  // The platform-specific implementation.
  service_impl_type service_impl_;
};

} // namespace asio

#include "RCF/external/asio/asio/detail/pop_options.hpp"

#endif // ASIO_STREAM_SOCKET_SERVICE_HPP
