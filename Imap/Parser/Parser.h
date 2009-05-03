/* Copyright (C) 2007 Jan Kundrát <jkt@gentoo.org>

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
#ifndef IMAP_PARSER_H
#define IMAP_PARSER_H
#include <deque>
#include <memory>
#include <tr1/memory>
#include <QObject>
#include <QMutex>
#include <QThread>
#include <QSemaphore>
#include "Response.h"
#include "Command.h"
#include "../Exceptions.h"
#include "../Streams/Socket.h"

/**
 * @file
 * A header file defining Parser class and various helpers.
 *
 * @author Jan Kundrát <jkt@gentoo.org>
 */

class ImapParserParseTest;
class QDateTime;
template<class T> class QList;

/** @short Namespace for IMAP interaction */
namespace Imap {

    /** @short Threading algorithm for THREAD command */
    enum ThreadAlgorithm {
        THREAD_NONE /**< No thread sorting (plain list of mails) */,
        THREAD_ORDEREDSUBJECT /**< ORDEREDSUBJECT algorithm */,
        THREAD_REFERENCES /**< REFERENCES algorithm */
    };

    Q_ENUMS( ThreadAlgorithm )

    /** @short Class specifying a set of messagess to access */
    class Sequence {
        uint _lo, _hi;
        bool _upperUnlimited;
    public:
        Sequence( const uint num ): _lo(num), _hi(num), _upperUnlimited(false) {};
        Sequence( const uint lo, const uint hi ): _lo(lo), _hi(hi), _upperUnlimited(false) {};
        static Sequence startingAt( const uint lo );

        /** @short Converts sequence to string suitable for sending over the wire */
        QString toString() const;
    };

    /** @short A handle identifying a command sent to the server */
    typedef QString CommandHandle;

    /** @short Class that does all IMAP parsing */
    class Parser : public QObject {
        Q_OBJECT

        friend class ::ImapParserParseTest;

    public:
        /** @short Constructor.
         *
         * Takes an QIODevice instance as a parameter. */
        Parser( QObject* parent, Imap::SocketPtr socket );

        /** @short Checks for waiting responses */
        bool hasResponse() const;

        /** @short De-queue and return parsed response */
        std::tr1::shared_ptr<Responses::AbstractResponse> getResponse();

        /** @short Enable/Disable sending literals using the LITERAL+ extension */
        void enableLiteralPlus( const bool enabled=true );

    public slots:

        /** @short CAPABILITY, RFC 3501 section 6.1.1 */
        CommandHandle capability();

        /** @short NOOP, RFC 3501 section 6.1.2 */
        CommandHandle noop();

        /** @short LOGOUT, RFC3501 section 6.1.3 */
        CommandHandle logout();


        /** @short STARTTLS, RFC3051 section 6.2.1 */
        CommandHandle startTls();

#if 0
        /** @short AUTHENTICATE, RFC3501 section 6.2.2 */
        CommandHandle authenticate( /* FIXME: parameter */ );
#endif

        /** @short LOGIN, RFC3501 section 6.2.3 */
        CommandHandle login( const QString& user, const QString& pass );


        /** @short SELECT, RFC3501 section 6.3.1 */
        CommandHandle select( const QString& mailbox );

        /** @short EXAMINE, RFC3501 section 6.3.2 */
        CommandHandle examine( const QString& mailbox );

        /** @short CREATE, RFC3501 section 6.3.3 */
        CommandHandle create( const QString& mailbox );

        /** @short DELETE, RFC3501 section 6.3.4 */
        CommandHandle deleteMailbox( const QString& mailbox );

        /** @short RENAME, RFC3501 section 6.3.5 */
        CommandHandle rename( const QString& oldName, const QString& newName );

        /** @short SUBSCRIBE, RFC3501 section 6.3.6 */
        CommandHandle subscribe( const QString& mailbox );
        
        /** @short UNSUBSCRIBE, RFC3501 section 6.3.7 */
        CommandHandle unSubscribe( const QString& mailbox );

        /** @short LIST, RFC3501 section 6.3.8 */
        CommandHandle list( const QString& reference, const QString& mailbox );
        
        /** @short LSUB, RFC3501 section 6.3.9 */
        CommandHandle lSub( const QString& reference, const QString& mailbox );

        /** @short STATUS, RFC3501 section 6.3.10 */
        CommandHandle status( const QString& mailbox, const QStringList& fields );
        
        /** @short APPEND, RFC3501 section 6.3.11 */
        CommandHandle append( const QString& mailbox, const QString& message,
                const QStringList& flags = QStringList(), const QDateTime& timestamp = QDateTime() );


        /** @short CHECK, RFC3501 sect 6.4.1 */
        CommandHandle check();

        /** @short CLOSE, RFC3501 sect 6.4.2 */
        CommandHandle close();
        
        /** @short EXPUNGE, RFC3501 sect 6.4.3 */
        CommandHandle expunge();

        /** @short SEARCH, RFC3501 sect 6.4.4 */
        CommandHandle search( const QStringList& criteria, const QString& charset = QString::null ) {
            return _searchHelper( "SEARCH", criteria, charset );
        };

        /** @short FETCH, RFC3501 sect 6.4.5 */
        CommandHandle fetch( const Sequence& seq, const QStringList& items );

        /** @short STORE, RFC3501 sect 6.4.6 */
        CommandHandle store( const Sequence& seq, const QString& item, const QString& value );

        /** @short COPY, RFC3501 sect 6.4.7 */
        CommandHandle copy( const Sequence& seq, const QString& mailbox );

        /** @short UID command (FETCH), RFC3501 sect 6.4.8 */
        CommandHandle uidFetch( const Sequence& seq, const QStringList& items );

        /** @short UID command (SEARCH), RFC3501 sect 6.4.8 */
        CommandHandle uidSearch( const QStringList& criteria, const QString& charset ) {
            return _searchHelper( "UID SEARCH", criteria, charset );
        };


        /** @short X<atom>, RFC3501 sect 6.5.1 */
        CommandHandle xAtom( const Commands::Command& commands );


        /** @short UNSELECT, RFC3691 */
        CommandHandle unSelect();
        
        /** @short IDLE, RFC2177

          The IDLE command will be automatically terminated by queueing of any command.
         */
        CommandHandle idle();


        /** @short NAMESPACE, RFC 2342 */
        CommandHandle namespaceCommand();


#if 0
        /** SORT, draft-ietf-imapext-sort-19, section 3 */
        CommandHandle sort( /*const SortAlgorithm& algo,*/ const QString& charset, const QStringList& criteria );
        /** UID SORT, draft-ietf-imapext-sort-19, section 3 */
        CommandHandle uidSort( /*const SortAlgorithm& algo,*/ const QString charset, const QStringList& criteria );
        /** THREAD, draft-ietf-imapext-sort-19, section 3 */
        CommandHandle thread( const ThreadAlgorithm& algo, const QString charset, const QStringList& criteria );
#endif

    signals:
        /** @short Socket got disconnected */
        void disconnected( const QString );

        /** @short New response received */
        void responseReceived();

        void commandQueued();

        void idleTerminated();

    private slots:
        void handleReadyRead();
        void executeACommand();
        void executeCommands();

    private:
        /** @short Private copy constructor */
        Parser( const Parser& );
        /** @short Private assignment operator */
        Parser& operator=( const Parser& );

        /** @short Queue command for execution.*/
        CommandHandle queueCommand( Commands::Command command );

        /** @short Shortcut function; works exactly same as above mentioned queueCommand() */
        CommandHandle queueCommand( const Commands::TokenType kind, const QString& text ) {
            return queueCommand( Commands::Command() << Commands::PartOfCommand( kind, text ) );
        };

        /** @short Helper for search() and uidSearch() */
        CommandHandle _searchHelper( const QString& command, const QStringList& criteria,
                const QString& charset = QString::null );

        /** @short Generate tag for next command */
        QString generateTag();

        void processLine( QByteArray line );

        /** @short Parse line for untagged reply */
        std::tr1::shared_ptr<Responses::AbstractResponse> parseUntagged( const QByteArray& line );

        /** @short Parse line for tagged reply */
        std::tr1::shared_ptr<Responses::AbstractResponse> parseTagged( const QByteArray& line );

        /** @short helper for parseUntagged() */
        std::tr1::shared_ptr<Responses::AbstractResponse> _parseUntaggedNumber(
                const QByteArray& line, int& start, const uint number );

        /** @short helper for parseUntagged() */
        std::tr1::shared_ptr<Responses::AbstractResponse> _parseUntaggedText(
                const QByteArray& line, int& start );

        /** @short Add parsed response to the internal queue, emit notification signal */
        void queueResponse( const std::tr1::shared_ptr<Responses::AbstractResponse>& resp );

        /** @short Connection to the IMAP server */
        SocketPtr _socket;

        /** @short Keeps track of the last-used command tag */
        unsigned int _lastTagUsed;

        /** @short Queue storing commands that are about to be executed */
        QList<Commands::Command> _cmdQueue;

        /** @short Queue storing parsed replies from the IMAP server */
        std::deque<std::tr1::shared_ptr<Responses::AbstractResponse> > _respQueue;

        bool _idling;

        bool _literalPlus;
        bool _waitingForContinuation;

        enum { ReadingLine, ReadingNumberOfBytes } _readingMode;
        QByteArray _currentLine;
        int _oldLiteralPosition;
        uint _readingBytes;

    };

    /** @short A convenience typedef */
    typedef std::tr1::shared_ptr<Parser> ParserPtr;

    QTextStream& operator<<( QTextStream& stream, const Sequence& s );

}
#endif /* IMAP_PARSER_H */
