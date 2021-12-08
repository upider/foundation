#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

namespace net
{
    class Protocol
    {
    public:
        Protocol();
        virtual ~Protocol();
        virtual int family() const = 0;
        virtual int type() const = 0;
        virtual int protocol() const = 0;
    };

    namespace tcp
    {

        class ProtocolV4 : public Protocol
        {
        public:
            ProtocolV4();
            virtual ~ProtocolV4();
            virtual int family() const;
            virtual int type() const;
            virtual int protocol() const;
        };

        class ProtocolV6 : public Protocol
        {
        public:
            ProtocolV6();
            virtual ~ProtocolV6();
            virtual int family() const;
            virtual int type() const;
            virtual int protocol() const;
        };
    } // namespace tcp

    namespace udp
    {
        class ProtocolV4 : public Protocol
        {
        public:
            ProtocolV4();
            virtual ~ProtocolV4();
            virtual int family() const;
            virtual int type() const;
            virtual int protocol() const;
        };

        class ProtocolV6 : public Protocol
        {
        public:
            ProtocolV6();
            virtual ~ProtocolV6();
            virtual int family() const;
            virtual int type() const;
            virtual int protocol() const;
        };
    } // namespace udp

} // namespace net

#endif /* __PROTOCOL_HPP__ */
