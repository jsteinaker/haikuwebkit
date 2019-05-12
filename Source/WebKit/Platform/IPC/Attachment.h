/*
 * Copyright (C) 2010-2020 Apple Inc. All rights reserved.
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if OS(DARWIN)
#include <wtf/MachSendRight.h>
#endif

#if USE(UNIX_DOMAIN_SOCKETS)
#include <optional>
#include <variant>
#include <wtf/Function.h>
#include <wtf/unix/UnixFileDescriptor.h>
#endif

#if PLATFORM(HAIKU)
#include <OS.h>
#include <String.h>
#endif

namespace IPC {

// IPC::Attachment is a type representing objects that cannot be transferred as data,
// rather they are transferred via operating system cross-process communication primitives.
#if OS(DARWIN)
using Attachment = MachSendRight;
#elif OS(WINDOWS)
using Attachment = int; // Windows does not need attachments at the moment.
#else
class Decoder;
class Encoder;

class Attachment {
public:
    Attachment();

    enum Type {
        Uninitialized,
        FileDescriptorType,
        CustomWriterType,
    };

    explicit Attachment(UnixFileDescriptor&&);

    Attachment(Attachment&&);
    Attachment& operator=(Attachment&&);
    ~Attachment();

    using SocketDescriptor = int;
    using CustomWriterFunc = WTF::Function<void(SocketDescriptor)>;
    using CustomWriter = std::variant<CustomWriterFunc, SocketDescriptor>;
    Attachment(CustomWriter&&);
#if PLATFORM(HAIKU)
	Attachment(team_id connectionInfo,uint32_t key)
		: m_connectionID(connectionInfo),
		m_key(key)
	{
	}    
#endif

    Type type() const { return m_type; }

    bool isNull() const { return !m_fd; }

    const UnixFileDescriptor& fd() const { return m_fd; }
    UnixFileDescriptor release() { return std::exchange(m_fd, UnixFileDescriptor { }); }

    const CustomWriter& customWriter() const { return m_customWriter; }
<<<<<<< HEAD
=======
#elif OS(DARWIN)
    void release();

    // MachPortType
    mach_port_name_t port() const { return m_port; }
    mach_msg_type_name_t disposition() const { return m_disposition; }
#elif OS(WINDOWS)
    HANDLE handle() const { return m_handle; }
#elif PLATFORM(HAIKU)
	team_id connectionID () const { return m_connectionID; }
	uint32_t key () const { return m_key; }
#endif
>>>>>>> c31f72459c (IPC for haiku)

    void encode(Encoder&) const;
    static std::optional<Attachment> decode(Decoder&);
private:
    Type m_type;

    UnixFileDescriptor m_fd;
    CustomWriter m_customWriter;
<<<<<<< HEAD
=======
#elif OS(DARWIN)
    mach_port_name_t m_port { 0 };
    mach_msg_type_name_t m_disposition { 0 };
#elif OS(WINDOWS)
    HANDLE m_handle { INVALID_HANDLE_VALUE };
#elif PLATFORM(HAIKU)
	team_id m_connectionID;
	uint32_t m_key;
#endif
>>>>>>> c31f72459c (IPC for haiku)
};
#endif

} // namespace IPC
