/* Copyright (C) 2007 - 2008 Jan Kundrát <jkt@gentoo.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef IMAP_MODEL_H
#define IMAP_MODEL_H

#include <QAbstractItemModel>
#include "../Cache.h"
#include "../Parser/Parser.h"
#include "../Streams/SocketFactory.h"

class QAuthenticator;

/** @short Namespace for IMAP interaction */
namespace Imap {

/** @short Classes for handling of mailboxes and connections */
namespace Mailbox {

class Model;

/** @short Response handler for implementing the State Pattern */
class ModelStateHandler: public QObject {
    Q_OBJECT
public:
    ModelStateHandler( Model* _m );

    virtual void handleState( Imap::ParserPtr ptr, const Imap::Responses::State* const resp ) = 0;
    virtual void handleCapability( Imap::ParserPtr ptr, const Imap::Responses::Capability* const resp ) = 0;
    virtual void handleNumberResponse( Imap::ParserPtr ptr, const Imap::Responses::NumberResponse* const resp ) = 0;
    virtual void handleList( Imap::ParserPtr ptr, const Imap::Responses::List* const resp ) = 0;
    virtual void handleFlags( Imap::ParserPtr ptr, const Imap::Responses::Flags* const resp ) = 0;
    virtual void handleSearch( Imap::ParserPtr ptr, const Imap::Responses::Search* const resp ) = 0;
    virtual void handleStatus( Imap::ParserPtr ptr, const Imap::Responses::Status* const resp ) = 0;
    virtual void handleFetch( Imap::ParserPtr ptr, const Imap::Responses::Fetch* const resp ) = 0;
    virtual void handleNamespace( Imap::ParserPtr ptr, const Imap::Responses::Namespace* const resp ) = 0;

protected:
    Model* m;
};

class UnauthenticatedHandler;
class AuthenticatedHandler;
class SyncingHandler;
class SelectedHandler;

class TreeItem;
class TreeItemMailbox;
class TreeItemMsgList;
class TreeItemMessage;
class TreeItemPart;
class MsgListModel;
class MailboxModel;

/** @short A model implementing view of the whole IMAP server */
class Model: public QAbstractItemModel {
    Q_OBJECT
    //Q_PROPERTY( ThreadAlgorithm threadSorting READ threadSorting WRITE setThreadSorting )

    struct Task {
        enum Kind { NONE, STARTTLS, LOGIN, LIST, STATUS, SELECT, FETCH };
        Kind kind;
        TreeItem* what;
        Task( const Kind _kind, TreeItem* _what ): kind(_kind), what(_what) {};
        Task(): kind(NONE) {};
    };

    enum RWMode { ReadOnly, ReadWrite };

    /** @short IMAP state of a connection */
    enum ConnectionState {
        CONN_STATE_ESTABLISHED /**< Connection established */,
        CONN_STATE_AUTH /**< We are authenticated, now we must select a mailbox */,
        CONN_STATE_SYNC /**< We are synchronizing a mailbox */,
        CONN_STATE_SELECTED /**< Some mailbox is selected */,
        CONN_STATE_LOGOUT /**< We have been logged out */
    };

    struct ParserState {
        struct SyncState {
            uint exists, recent, unSeen, uidNext, uidValidity;
            QList<QByteArray> flags, permanentFlags;
            SyncState(): exists(0), recent(0), unSeen(0), uidNext(0), uidValidity(0) {};
        };

        ParserPtr parser;
        TreeItemMailbox* mailbox;
        RWMode mode;
        ConnectionState connState;
        TreeItemMailbox* handler;
        QMap<CommandHandle, Task> commandMap;
        QStringList capabilities;
        bool capabilitiesFresh;
        QList<Responses::List> listResponses;
        QList<Responses::Status> statusResponses;
        SyncState syncState;
        ModelStateHandler* responseHandler;

        ParserState( ParserPtr _parser, TreeItemMailbox* _mailbox, const RWMode _mode, 
                const ConnectionState _connState, ModelStateHandler* _respHandler ):
            parser(_parser), mailbox(_mailbox), mode(_mode),
            connState(_connState), handler(0), capabilitiesFresh(false),
            responseHandler(_respHandler) {};
        ParserState(): mailbox(0), mode(ReadOnly), connState(CONN_STATE_LOGOUT),
            handler(0), capabilitiesFresh(false), responseHandler(0) {};
    };

    /** @short Policy for accessing network */
    enum NetworkPolicy {
        /**< @short No access to the network at all

        All network activity is suspended. If an action requires network access,
        it will either fail or be queued for later. */
        NETWORK_OFFLINE,
        /** @short Connections are possible, but expensive

        Information that is cached is preferred, as long as it is usable.
        Trojita will never miss a mail in this mode, but for example it won't
        check for new mailboxes. */
        NETWORK_EXPENSIVE,
        /** @short Connections have zero cost

        Normal mode of operation. All network activity is assumed to have zero
        cost and Trojita is free to ask network as often as possible. It will
        still use local cache when it makes sense, though. */
        NETWORK_ONLINE
    };

    mutable CachePtr _cache;
    mutable SocketFactoryPtr _socketFactory;
    mutable QMap<Parser*,ParserState> _parsers;
    int _maxParsers;
    mutable TreeItemMailbox* _mailboxes;
    mutable NetworkPolicy _netPolicy;
    bool _startTls;

    mutable QList<Imap::Responses::NamespaceData> _personalNamespace, _otherUsersNamespace, _sharedNamespace;


public:
    Model( QObject* parent, CachePtr cache, SocketFactoryPtr socketFactory );
    ~Model();

    virtual QModelIndex index(int row, int column, const QModelIndex& parent ) const;
    virtual QModelIndex parent(const QModelIndex& index ) const;
    virtual int rowCount(const QModelIndex& index ) const;
    virtual int columnCount(const QModelIndex& index ) const;
    virtual QVariant data(const QModelIndex& index, int role ) const;
    virtual bool hasChildren( const QModelIndex& parent = QModelIndex() ) const;

    void handleState( Imap::ParserPtr ptr, const Imap::Responses::State* const resp );
    void handleCapability( Imap::ParserPtr ptr, const Imap::Responses::Capability* const resp );
    void handleNumberResponse( Imap::ParserPtr ptr, const Imap::Responses::NumberResponse* const resp );
    void handleList( Imap::ParserPtr ptr, const Imap::Responses::List* const resp );
    void handleFlags( Imap::ParserPtr ptr, const Imap::Responses::Flags* const resp );
    void handleSearch( Imap::ParserPtr ptr, const Imap::Responses::Search* const resp );
    void handleStatus( Imap::ParserPtr ptr, const Imap::Responses::Status* const resp );
    void handleFetch( Imap::ParserPtr ptr, const Imap::Responses::Fetch* const resp );
    void handleNamespace( Imap::ParserPtr ptr, const Imap::Responses::Namespace* const resp );

    CachePtr cache() const { return _cache; };


public slots:
    void reloadMailboxList();

    void setNetworkOffline() { setNetworkPolicy( NETWORK_OFFLINE ); }
    void setNetworkExpensive() { setNetworkPolicy( NETWORK_EXPENSIVE ); }
    void setNetworkOnline() { setNetworkPolicy( NETWORK_ONLINE ); }

private slots:
    void slotParserDisconnected( const QString );

signals:
    void alertReceived( const QString& message );
    void networkPolicyOffline();
    void networkPolicyExpensive();
    void networkPolicyOnline();
    void connectionError( const QString& message );
    void authRequested( QAuthenticator* auth );

private:
    Model& operator=( const Model& ); // don't implement
    Model( const Model& ); // don't implement


    friend class TreeItem;
    friend class TreeItemMailbox;
    friend class TreeItemMsgList;
    friend class TreeItemMessage;
    friend class TreeItemPart;
    friend class MsgListModel; // needs access to createIndex()
    friend class MailboxModel; // needs access to createIndex()

    friend class UnauthenticatedHandler;
    friend class AuthenticatedHandler;
    friend class SelectedHandler;

    void _askForChildrenOfMailbox( TreeItemMailbox* item );
    void _askForMessagesInMailbox( TreeItemMsgList* item );
    void _askForMsgMetadata( TreeItemMessage* item );
    void _askForMsgPart( TreeItemPart* item );

    void _finalizeList( ParserPtr parser, const QMap<CommandHandle, Task>::const_iterator command );
    void _finalizeStatus( ParserPtr parser, const QMap<CommandHandle, Task>::const_iterator command );
    void _finalizeSelect( ParserPtr parser, const QMap<CommandHandle, Task>::const_iterator command );
    void _finalizeFetch( ParserPtr parser, const QMap<CommandHandle, Task>::const_iterator command );

    void replaceChildMailboxes( ParserPtr parser, TreeItemMailbox* mailboxPtr, const QList<TreeItem*> mailboxes );

    TreeItem* translatePtr( const QModelIndex& index ) const;

    /** @short Returns parser suitable for dealing with some mailbox.
     *
     * This parser might be already working hard in another mailbox; if that is
     * the case, it is asked to switch to the correct one.
     *
     * If allowed by policy, new parser might be created in the background.
     * */
    ParserPtr _getParser( TreeItemMailbox* mailbox, const RWMode rw ) const;

    NetworkPolicy networkPolicy() const { return _netPolicy; };
    void setNetworkPolicy( const NetworkPolicy policy );

    void completelyReset();

    ModelStateHandler* unauthHandler;
    ModelStateHandler* authenticatedHandler;
    ModelStateHandler* selectedHandler;

protected slots:
    void responseReceived();

};

}

}

#endif /* IMAP_MODEL_H */
