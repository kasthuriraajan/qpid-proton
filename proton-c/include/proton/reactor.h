#ifndef PROTON_REACTOR_H
#define PROTON_REACTOR_H 1

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include <proton/import_export.h>
#include <proton/type_compat.h>
#include <proton/error.h>
#include <proton/event.h>
#include <proton/selectable.h>
#include <proton/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond INTERNAL
 */

typedef struct pn_reactor_t pn_reactor_t;
typedef struct pn_acceptor_t pn_acceptor_t;
typedef struct pn_timer_t pn_timer_t;
typedef struct pn_task_t pn_task_t;

PNX_EXTERN pn_handler_t *pn_handler(void (*dispatch)(pn_handler_t *, pn_event_t *, pn_event_type_t));
PNX_EXTERN pn_handler_t *pn_handler_new(void (*dispatch)(pn_handler_t *, pn_event_t *, pn_event_type_t), size_t size,
                                       void (*finalize)(pn_handler_t *));
PNX_EXTERN void pn_handler_free(pn_handler_t *handler);
PNX_EXTERN void *pn_handler_mem(pn_handler_t *handler);
PNX_EXTERN void pn_handler_add(pn_handler_t *handler, pn_handler_t *child);
PNX_EXTERN void pn_handler_clear(pn_handler_t *handler);
PNX_EXTERN void pn_handler_dispatch(pn_handler_t *handler, pn_event_t *event, pn_event_type_t type);

PNX_EXTERN pn_reactor_t *pn_reactor(void);
PNX_EXTERN pn_record_t *pn_reactor_attachments(pn_reactor_t *reactor);
PNX_EXTERN pn_millis_t pn_reactor_get_timeout(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_set_timeout(pn_reactor_t *reactor, pn_millis_t timeout);
PNX_EXTERN pn_timestamp_t pn_reactor_mark(pn_reactor_t *reactor);
PNX_EXTERN pn_timestamp_t pn_reactor_now(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_yield(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_free(pn_reactor_t *reactor);
PNX_EXTERN pn_collector_t *pn_reactor_collector(pn_reactor_t *reactor);
PNX_EXTERN pn_handler_t *pn_reactor_get_global_handler(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_set_global_handler(pn_reactor_t *reactor, pn_handler_t *handler);
PNX_EXTERN pn_handler_t *pn_reactor_get_handler(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_set_handler(pn_reactor_t *reactor, pn_handler_t *handler);
PNX_EXTERN pn_list_t *pn_reactor_children(pn_reactor_t *reactor);
PNX_EXTERN pn_selectable_t *pn_reactor_selectable(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_update(pn_reactor_t *reactor, pn_selectable_t *selectable);
PNX_EXTERN pn_acceptor_t *pn_reactor_acceptor(pn_reactor_t *reactor, const char *host, const char *port,
                                             pn_handler_t *handler);
PNX_EXTERN pn_error_t *pn_reactor_error(pn_reactor_t *reactor);

/**
 * Create an outgoing connection that will be managed by the reactor.
 *
 * The reator's pn_iohandler will create a socket connection to the host
 * once the connection is opened.
 *
 * @param[in] reactor the reactor that will own the connection.
 * @param[in] host the address of the remote host. e.g. "localhost"
 * @param[in] port the port to connect to. e.g. "5672"
 * @param[in] handler the handler that will process all events generated by
 * this connection.
 * @return a connection object
 */
PNX_EXTERN pn_connection_t *pn_reactor_connection_to_host(pn_reactor_t *reactor,
                                                         const char *host,
                                                         const char *port,
                                                         pn_handler_t *handler);

/**
 * **Deprecated** - Use ::pn_reactor_connection_to_host().
 *
 * Create an outgoing connection that will be managed by the reactor.
 *
 * The host address for the connection must be set via
 * ::pn_reactor_set_connection_host() prior to opening the connection.
 * Typically this can be done by the handler when processing the
 * ::PN_CONNECTION_INIT event.
 *
 * @param[in] reactor the reactor that will own the connection.
 * @param[in] handler the handler that will process all events generated by
 * this connection.
 * @return a connection object
 */
PNX_EXTERN pn_connection_t *pn_reactor_connection(pn_reactor_t *reactor,
                                                 pn_handler_t *handler);

/**
 * Change the host address used by an outgoing reactor connection.
 *
 * The address is used by the reactor's iohandler to create an outgoing socket
 * connection.  This must be set prior to (re)opening the connection.
 *
 * @param[in] reactor the reactor that owns the connection.
 * @param[in] connection the connection created by the reactor.
 * @param[in] host the network address or DNS name of the host to connect to.
 * @param[in] port the network port to use. Optional - default is "5672"
 */
PNX_EXTERN void pn_reactor_set_connection_host(pn_reactor_t *reactor,
                                              pn_connection_t *connection,
                                              const char *host,
                                              const char *port);
/**
 * Retrieve the peer host address for a reactor connection.
 *
 * This may be used to retrieve the host address used by the reactor to
 * establish the outgoing socket connection.  In the case of an accepted
 * connection the returned value is the address of the remote.
 *
 * @note Note that the returned address may be in numeric IP format.
 *
 * The pointer returned by this operation is valid until either the address is
 * changed via ::pn_reactor_set_connection_host() or the connection object
 * is freed.
 *
 * @param[in] reactor the reactor that owns the connection.
 * @param[in] connection the reactor connection
 * @return a C string containing the address in URL format or NULL if no
 * address available.  ::pn_url_parse() may be used to create a Proton pn_url_t
 * instance from the returned value.
 */
PNX_EXTERN const char *pn_reactor_get_connection_address(pn_reactor_t *reactor,
                                                        pn_connection_t *connection);

PNX_EXTERN int pn_reactor_wakeup(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_start(pn_reactor_t *reactor);
PNX_EXTERN bool pn_reactor_quiesced(pn_reactor_t *reactor);
PNX_EXTERN bool pn_reactor_process(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_stop(pn_reactor_t *reactor);
PNX_EXTERN void pn_reactor_run(pn_reactor_t *reactor);
PNX_EXTERN pn_task_t *pn_reactor_schedule(pn_reactor_t *reactor, int delay, pn_handler_t *handler);


PNX_EXTERN void pn_acceptor_set_ssl_domain(pn_acceptor_t *acceptor, pn_ssl_domain_t *domain);
PNX_EXTERN void pn_acceptor_close(pn_acceptor_t *acceptor);
PNX_EXTERN pn_acceptor_t *pn_connection_acceptor(pn_connection_t *connection);

PNX_EXTERN pn_timer_t *pn_timer(pn_collector_t *collector);
PNX_EXTERN pn_timestamp_t pn_timer_deadline(pn_timer_t *timer);
PNX_EXTERN void pn_timer_tick(pn_timer_t *timer, pn_timestamp_t now);
PNX_EXTERN pn_task_t *pn_timer_schedule(pn_timer_t *timer, pn_timestamp_t deadline);
PNX_EXTERN int pn_timer_tasks(pn_timer_t *timer);

PNX_EXTERN pn_record_t *pn_task_attachments(pn_task_t *task);
PNX_EXTERN void pn_task_cancel(pn_task_t *task);

PNX_EXTERN pn_reactor_t *pn_class_reactor(const pn_class_t *clazz, void *object);
PNX_EXTERN pn_reactor_t *pn_object_reactor(void *object);
PNX_EXTERN pn_reactor_t *pn_event_reactor(pn_event_t *event);

PNX_EXTERN pn_handler_t *pn_record_get_handler(pn_record_t *record);
PNX_EXTERN void pn_record_set_handler(pn_record_t *record, pn_handler_t *handler);

/**
 * Get the root handler the current event was dispatched to.
 */
PNX_EXTERN pn_handler_t *pn_event_root(pn_event_t *event);

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif

#endif /* reactor.h */
