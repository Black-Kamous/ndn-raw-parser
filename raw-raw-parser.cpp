#include <ndn-cxx/encoding/block.hpp>
#include <ndn-cxx/interest.hpp>
#include <iostream>
#include <iomanip>
#include <queue>

namespace ndn{

using BlockContainer = std::queue<Block>;
using BufferContainer = std::queue<ConstBufferPtr>;

class BlockParser {
public:
    

    BlockParser(){}

    /*
     * 将buf，即wire format添加到第一个队列中
    */
    void
    addBuf(ConstBufferPtr buf)
    {
        m_unrecog.push(std::move(buf));
    }

    /*
     * 使用Block::fromBuffer解析wire format，转移到第二个队列中
    */
    void
    decode()
    {
        auto [isOk, block] = Block::fromBuffer(m_unrecog.front());
        if(!isOk)
            return;
        m_unparsed.push(std::move(block));
        m_unrecog.pop();
    }

    
    void
    showRecognized()
    {
        auto b = m_unparsed.front();
        std::cout << "has wire " << b.hasWire() << " has value " << b.hasValue() 
        << " is valid " << b.isValid() << " size " << b.value_size() 
        << " type " << b.type() << std::endl;
    }

    /*
     * 使用parse()解析sub-elements
    */
    void
    parse()
    {
        m_unparsed.front().parse();
        // auto ptr = m_unparsed.front().elements_begin();
        // ptr->parse();
        m_parsed.push(m_unparsed.front());
        m_unparsed.pop();
    }

private:
    BufferContainer m_unrecog;
    BlockContainer m_unparsed;
    BlockContainer m_parsed;
};

class BlockEncoder
{
public:
    void
    makeInterest()
    {
        static int cnt = 1;
        Name interestName("/example" + std::to_string(cnt++) + "/testApp/randomData");
        interestName.appendVersion();

        Interest interest(interestName);
        interest.setMustBeFresh(true);
        interest.setInterestLifetime(6_s);

        m_interest.push(interest);
    }

    void
    encode()
    {
        const Block& b = m_interest.front().wireEncode();
        m_block.push(b);
        m_interest.pop();

    }

    void
    showEncode()
    {
        auto b = m_block.front();
        std::cout << "has wire " << b.hasWire() << " has value " << b.hasValue() 
        << " is valid " << b.isValid() << " size " << b.value_size() 
        << " type " << b.type() << std::endl;
        auto buf = b.getBuffer();
        for (auto it = buf->begin();it!=buf->end();++it)
        {
            std::cout << std::setfill('0') << std::hex << std::setw(2) << (int)*it;
        }
        std::cout << std::endl;
    }

    Block
    getEncoded()
    {
        Block b = std::move(m_block.front());
        m_block.pop();
        return std::move(b);
    }

private:
    std::queue<Interest> m_interest;
    BlockContainer m_block;
};

}

void
hexize(char* src, int len, char* dst)
{
    static std::map<char, char> cmap = {{'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};
    if(len%2 != 0)
        return;
    int st = 0;
    while(len - st)
    {
        dst[st/2] = cmap[src[st]]*16 + cmap[src[st+1]];
        st+=2;
    }
}

int main(int argc, char** args)
{
    char w[]=
    "06c5072808076578616d706c6508"
    "0774657374417070080a72616e646f6d"
    "4461746136080000018d6e064d121404"
    "19022710150d48656c6c6f2c20776f72"
    "6c6421163b1b01031c36073408076578"
    "616d706c650807746573744170700803"
    "4b45590808097f972a30168909080765"
    "78616d706c6536080000018cd2acad37"
    "1747304502207d19ebd7700949e82269"
    "5d267176797b760771b1c7b09307d804"
    "7c0b8c8201ee0221008086aa3fb40bec"
    "d3cf33b4871e3ab80f37c5664ce53753"
    "5404a1810c6a647c00";

    char whex[200] = {0};

    hexize(w, 398, whex);


    ndn::Buffer wirebuf(whex, 211);
    ndn::BlockParser bp;
    ndn::BlockParser(std::move(bp));
    ndn::BlockEncoder be;

    try{
        for(int i=0;i<10000;i++){
            be.makeInterest();
            be.encode();
            //be.showEncode();

            bp.addBuf(be.getEncoded().getBuffer());
            bp.decode();
            //bp.showRecognized();
            bp.parse();
        }
        
    }catch(ndn::Block::Error& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }


    return 0;
}