#ifndef CELLSTREAM_H
#define CELLSTREAM_H
#include <cstdint>


class CELLStream
{
    CELLStream(int nSize = 1024)
    {
        m_nSize = nSize;
        m_pBuff = new char[nSize];
        m_bDelete = true;
    }

    CELLStream(char* pData , int nSize ,bool bDelete = false)
    {
        m_nSize = nSize;
        m_pBuff = pData;
        m_bDelete = bDelete;
    }

    ~CELLStream()
    {
        if(m_pBuff && m_bDelete)
        {
            delete[] m_pBuff;
            m_pBuff = nullptr;
        }
    }

public:

//    int16_t readInt16();
//    int32_t readInt32();
//    float readFloat();
//    double readDouble();
//    bool writeFloat(float n);
//    bool writeDouble(double n);
//    bool writeInt8(int8_t n);
//    bool writeInt16(int16_t n);

    bool writeInt32(int32_t n)
    {
        return Write<int32_t>(n);
    }

    int8_t readInt8(int8_t def = 0)
    {
        Read<int8_t>(def);
        return def;
    }

    // 内联函数
    inline bool canRead(int n)
    {
        return  m_nSize - m_lastReadBuffPos >= n;
    }


    inline bool canWrite(int n)
    {
        return  m_nSize - m_lastWriteBuffPos >= n;
    }


    char* Data()
    {
        return m_pBuff;
    }


public:
    template<typename T>
    bool Write(T n)
    {
        auto nLen = sizeof(T);

        if(canWrite(nLen))
        {
            memcpy(m_pBuff +m_lastWriteBuffPos ,&n ,nLen );
            m_lastWriteBuffPos += nLen;

            return true;
        }

        return false;
    }

    template<typename T>
    bool writeArray(T* pData , uint32_t len)
    {
        auto nLen = sizeof(T)*len;

        if(canWrite (nLen + sizeof(uint32_t)))
        {
            //数组需要先写入数组的长度
            writeInt32(len);

            memcpy(m_pBuff +m_lastWriteBuffPos ,pData ,len );
            m_lastWriteBuffPos += len;

            return true;
        }

        return false;
    }

//    template<typename T>
//    bool Read(T& n)
//    {

//        auto nLen = sizeof(T);

//        if(canRead(nLen))
//        {
//            memcpy(&n , m_pBuff +m_lastReadBuffPos ,nLen );
//            m_lastReadBuffPos += nLen;

//            return true;
//        }

//        return false;
//    }

    template<typename T>
    bool Read(T& n ,bool isOffset = true)
    {

        auto nLen = sizeof(T);

        if(m_lastReadBuffPos + nLen <= m_nSize)
        {
            memcpy(&n , m_pBuff +m_lastReadBuffPos ,nLen );

            if(isOffset)
                m_lastReadBuffPos += nLen;

            return true;
        }

        return false;
    }

    template<typename T>
    uint32_t ReadArray(T* pArr , uint32_t len)
    {

        uint32_t ReadCount = 0;
        Read(ReadCount,false);

        if(ReadCount < len)
        {
            auto nLen = sizeof(T) * ReadCount;

            if(canRead( nLen + sizeof(uint32_t)))
            {
                //上面成功，不偏移，在这里偏移
                m_lastReadBuffPos += sizeof(uint32_t);

                memcpy(pArr , m_pBuff +m_lastReadBuffPos ,nLen );

                m_lastReadBuffPos += nLen;

                return ReadCount;
            }
        }

        return 0;
    }

private:
    //缓冲区
    char* m_pBuff = nullptr;

    //数据尾部位置
    int m_lastWriteBuffPos =0;

    //数据尾部位置
    int m_lastReadBuffPos =0;

    int m_nSize=0;

    int m_fullCount=0;

    bool m_bDelete = false;

};

#endif // CELLSTREAM_H
