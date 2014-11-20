
// 定式:
build: 从pattern构建DFA
parse: 解析str

// parser

Lexer lexer;
lexer.add(NUM, "[1-9][0-9]*");
lexer.add(KEY, "if");
lexer.add(ID, "[_a-zA-Z][_a-zA-Z0-9]*")
lexer.build();

lexer.parse(reader);

while (lexer.lookahead().type != TT_EOS) {
    token = lexer.next();
}

// lexer
DFAGenerator _dfa;
DFAMerger _merger;

add:
_dfa.parse(NUM, num_re);
_merger.add(gen);

build:
_merger.build();
_dfa.build(merger);



// DFA状态转义明确,D
typedef std::map<char, int> DFATran;
// 只要是DFA,就能用于处理,min与否无所谓
typedef std::vector<int> Tag;

class DFA {
public:
    virtual ~DFA();
    virtual size_t size() = 0;
    virtual const DFATran& tran(size_t s) = 0;
    virtual const Tag& tag(size_t) = 0;
};

// NFA -> DFA
class DFAConverter : public DFA {
public:
    void parse(const char* pattern, int tag) {
        NFAConverter nfa;
        nfa.parse(pattern);
        std::map<int, Tag> tags;
        tags[nfa.last()].push_back(tag);
        build(nfa.start(), nfa.last(), nfa.trans(), tags);
    }

    // 和nfa结果类似,区别:
    // 1. 除了start/last,没有'\0'
    // 2. Tran是确定的,但nfa则是vector
    // 如何融合二者的算法?
    // 1. 将DFATran -> NFATran, 生成vector
    void build(int start, int last,
            const std::vector<NFATran>& trans,
            const std::vector<Tag>& tags);
};

// DFA -> min
class DFAGenerator : public DFA {
public:
    void parse(const char* pattern, int tag) {
        DFAConverter dfa;
        dfa.parse(pattern, tag);
        build(dfa);
    }

    void build(const DFA& dfa);
};

// DFAs -> NFA -> DFA
class DFAMerger : public DFA {
public:
    // 这里直接转成NFA是否合适?
    // 合适,直接转换NFATran
    void add(const DFA& dfa) {
        size_t size = dfa.size();
        size_t base = _trans.size();
        _trans.resize(_trans.size() + size);
        for (size_t i = 0; i < size; i++) {
            const DFATran& tran = dfa.trans(i);
            for (DFATran::const_iterator it = tran.begin();
                    it != tran.end(); ++it) {
                _trans[base + i][it->first] = base + it->second;
            }
            const Tag& tag = dfa.tags(i);
            if (!tag.empty()) {
                _tags[base + i].insert(tag.begin(), tag.end());
            }
        }
        _trans[_start]['\0'].push_back(base + dfa.start());
        const std::vector<int>& last = dfa.last();
        for (size_t i = 0; i < last.size(); i++) {
            _trans[base + last[i]]['\0'].push_back(_last);
        }
    }

    void build() {
        _dfa.build(_start, _last, _trans, _tags);
    }

    std::vector<NFATran> _trans;
    std::vector<int> _tags;

    DFAConverter _dfa;
    int _start;
    int _last;
};
