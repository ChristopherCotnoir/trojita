/* Copyright (C) 2006 - 2013 Jan Kundrát <jkt@flaska.net>

   This file is part of the Trojita Qt IMAP e-mail client,
   http://trojita.flaska.net/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TaskFactory.h"
#include "AppendTask.h"
#include "CopyMoveMessagesTask.h"
#include "CreateMailboxTask.h"
#include "DeleteMailboxTask.h"
#include "EnableTask.h"
#include "ExpungeMailboxTask.h"
#include "FetchMsgMetadataTask.h"
#include "FetchMsgPartTask.h"
#include "GenUrlAuthTask.h"
#include "GetAnyConnectionTask.h"
#include "IdTask.h"
#include "KeepMailboxOpenTask.h"
#include "Fake_ListChildMailboxesTask.h"
#include "Fake_OpenConnectionTask.h"
#include "NumberOfMessagesTask.h"
#include "ObtainSynchronizedMailboxTask.h"
#include "OpenConnectionTask.h"
#include "UidSubmitTask.h"
#include "UpdateFlagsTask.h"
#include "ThreadTask.h"
#include "NoopTask.h"
#include "UnSelectTask.h"
#include "SortTask.h"
#include "SubscribeUnsubscribeTask.h"
#include "Imap/Model/TaskPresentationModel.h"
#include "Imap/Parser/Parser.h"
#include "Streams/SocketFactory.h"

namespace Imap
{
namespace Mailbox
{

TaskFactory::~TaskFactory()
{
}

OpenConnectionTask *TaskFactory::createOpenConnectionTask(Model *model)
{
    return new OpenConnectionTask(model);
}

CopyMoveMessagesTask *TaskFactory::createCopyMoveMessagesTask(Model *model, const QModelIndexList &messages,
        const QString &targetMailbox, const CopyMoveOperation op)
{
    return new CopyMoveMessagesTask(model, messages, targetMailbox, op);
}

CreateMailboxTask *TaskFactory::createCreateMailboxTask(Model *model, const QString &mailbox)
{
    return new CreateMailboxTask(model, mailbox);
}

GetAnyConnectionTask *TaskFactory::createGetAnyConnectionTask(Model *model)
{
    return new GetAnyConnectionTask(model);
}

ListChildMailboxesTask *TaskFactory::createListChildMailboxesTask(Model *model, const QModelIndex &mailbox)
{
    return new ListChildMailboxesTask(model, mailbox);
}

DeleteMailboxTask *TaskFactory::createDeleteMailboxTask(Model *model, const QString &mailbox)
{
    return new DeleteMailboxTask(model, mailbox);
}

ExpungeMailboxTask *TaskFactory::createExpungeMailboxTask(Model *model, const QModelIndex &mailbox)
{
    return new ExpungeMailboxTask(model, mailbox);
}

FetchMsgMetadataTask *TaskFactory::createFetchMsgMetadataTask(Model *model, const QModelIndex &mailbox, const QList<uint> &uids)
{
    return new FetchMsgMetadataTask(model, mailbox, uids);
}

FetchMsgPartTask *TaskFactory::createFetchMsgPartTask(Model *model, const QModelIndex &mailbox, const QList<uint> &uids, const QStringList &parts)
{
    return new FetchMsgPartTask(model, mailbox, uids, parts);
}

IdTask *TaskFactory::createIdTask(Model *model, ImapTask *dependingTask)
{
    return new IdTask(model, dependingTask);
}

EnableTask *TaskFactory::createEnableTask(Model *model, ImapTask *dependingTask, const QList<QByteArray> &extensions)
{
    return new EnableTask(model, dependingTask, extensions);
}

KeepMailboxOpenTask *TaskFactory::createKeepMailboxOpenTask(Model *model, const QModelIndex &mailbox, Parser *oldParser)
{
    return new KeepMailboxOpenTask(model, mailbox, oldParser);
}

NumberOfMessagesTask *TaskFactory::createNumberOfMessagesTask(Model *model, const QModelIndex &mailbox)
{
    return new NumberOfMessagesTask(model, mailbox);
}

ObtainSynchronizedMailboxTask *TaskFactory::createObtainSynchronizedMailboxTask(Model *model, const QModelIndex &mailboxIndex,
        ImapTask *parentTask, KeepMailboxOpenTask *keepTask)
{
    return new ObtainSynchronizedMailboxTask(model, mailboxIndex, parentTask, keepTask);
}

UpdateFlagsTask *TaskFactory::createUpdateFlagsTask(Model *model, const QModelIndexList &messages, const FlagsOperation flagOperation, const QString &flags)
{
    return new UpdateFlagsTask(model, messages, flagOperation, flags);
}

UpdateFlagsTask *TaskFactory::createUpdateFlagsTask(Model *model, CopyMoveMessagesTask *copyTask, const QList<QPersistentModelIndex> &messages, const FlagsOperation flagOperation, const QString &flags)
{
    return new UpdateFlagsTask(model, copyTask, messages, flagOperation, flags);
}

ThreadTask *TaskFactory::createThreadTask(Model *model, const QModelIndex &mailbox, const QByteArray &algorithm, const QStringList &searchCriteria)
{
    return new ThreadTask(model, mailbox, algorithm, searchCriteria);
}

ThreadTask *TaskFactory::createIncrementalThreadTask(Model *model, const QModelIndex &mailbox, const QByteArray &algorithm, const QStringList &searchCriteria)
{
    return new ThreadTask(model, mailbox, algorithm, searchCriteria, ThreadTask::THREADING_INCREMENTAL);
}

NoopTask *TaskFactory::createNoopTask(Model *model, ImapTask *parentTask)
{
    return new NoopTask(model, parentTask);
}

UnSelectTask *TaskFactory::createUnSelectTask(Model *model, ImapTask *parentTask)
{
    return new UnSelectTask(model, parentTask);
}

SortTask *TaskFactory::createSortTask(Model *model, const QModelIndex &mailbox, const QStringList &searchConditions, const QStringList &sortCriteria)
{
    return new SortTask(model, mailbox, searchConditions, sortCriteria);
}

AppendTask *TaskFactory::createAppendTask(Model *model, const QString &targetMailbox, const QByteArray &rawMessageData,
                                          const QStringList &flags, const QDateTime &timestamp)
{
    return new AppendTask(model, targetMailbox, rawMessageData, flags, timestamp);
}

AppendTask *TaskFactory::createAppendTask(Model *model, const QString &targetMailbox, const QList<CatenatePair> &data,
                                          const QStringList &flags, const QDateTime &timestamp)
{
    return new AppendTask(model, targetMailbox, data, flags, timestamp);
}

SubscribeUnsubscribeTask *TaskFactory::createSubscribeUnsubscribeTask(Model *model, const QModelIndex &mailbox,
                                                                      const SubscribeUnsubscribeOperation operation)
{
    return new SubscribeUnsubscribeTask(model, mailbox, operation);
}

GenUrlAuthTask *TaskFactory::createGenUrlAuthTask(Model *model, const QString &host, const QString &user, const QString &mailbox,
                                                  const uint uidValidity, const uint uid, const QString &part, const QString &access)
{
    return new GenUrlAuthTask(model, host, user, mailbox, uidValidity, uid, part, access);
}
UidSubmitTask *TaskFactory::createUidSubmitTask(Model *model, const QString &mailbox, const uint uidValidity, const uint uid,
                                                const UidSubmitOptionsList &submitOptions)
{
    return new UidSubmitTask(model, mailbox, uidValidity, uid, submitOptions);
}

TestingTaskFactory::TestingTaskFactory(): TaskFactory(), fakeOpenConnectionTask(false), fakeListChildMailboxes(false)
{
}

Parser *TestingTaskFactory::newParser(Model *model)
{
    Parser *parser = new Parser(model, model->m_socketFactory->create(), ++model->m_lastParserId);
    ParserState parserState(parser);
    QObject::connect(parser, SIGNAL(responseReceived(Imap::Parser*)), model, SLOT(responseReceived(Imap::Parser*)), Qt::QueuedConnection);
    QObject::connect(parser, SIGNAL(connectionStateChanged(Imap::Parser*,Imap::ConnectionState)), model, SLOT(handleSocketStateChanged(Imap::Parser*,Imap::ConnectionState)));
    QObject::connect(parser, SIGNAL(lineReceived(Imap::Parser*,QByteArray)), model, SLOT(slotParserLineReceived(Imap::Parser*,QByteArray)));
    QObject::connect(parser, SIGNAL(lineSent(Imap::Parser*,QByteArray)), model, SLOT(slotParserLineSent(Imap::Parser*,QByteArray)));
    model->m_parsers[ parser ] = parserState;
    model->m_taskModel->slotParserCreated(parser);
    return parser;
}

OpenConnectionTask *TestingTaskFactory::createOpenConnectionTask(Model *model)
{
    if (fakeOpenConnectionTask) {
        return new Fake_OpenConnectionTask(model, newParser(model));
    } else {
        return TaskFactory::createOpenConnectionTask(model);
    }
}

ListChildMailboxesTask *TestingTaskFactory::createListChildMailboxesTask(Model *model, const QModelIndex &mailbox)
{
    if (fakeListChildMailboxes) {
        return new Fake_ListChildMailboxesTask(model, mailbox);
    } else {
        return TaskFactory::createListChildMailboxesTask(model, mailbox);
    }
}

}
}
