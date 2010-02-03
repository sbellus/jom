all: ppexpr_grammar.cpp

ppexpr-lex.inc: ppexpr.l
    flex ppexpr.l

ppexpr_grammar.cpp: ppexpr.g ppexpr-lex.inc
    qlalr --no-lines ppexpr.g
