
/* identifier */
Tok(TkLitIdent)
Tok(TkLitInteger)
Tok(TkLitFloat)
Tok(TkLitString)
Tok(TkLitChar)
/* keyword-ish literals */
TokKw(TkTrue,          true      )
TokKw(TkFalse,         false     )
TokKw(TkNull,          null      )
/* keywords */
TokKw(TkKwHashPackage, #package  )
TokKw(TkKwHashImport,  #import   )
TokKw(TkKwHashIf,      #if       )
TokKw(TkKwHashNull,    #null     )

TokKw(TkKWVar,         var       )
TokKw(TkKwConst,       const     )
TokKw(TkKwAs,          as        )
TokKw(TkKwInline,      inline    )

TokKw(TkKwAnd,         and       )
TokKw(TkKwOr,          or        )
TokKw(TkKwOrElse,      orelse    )

TokKw(TkKwProc,        proc      )
TokKw(TkKwReturn,      return    )
TokKw(TkKwDefer,       defer     )


TokKw(TkKwIf,         if         )
TokKw(TkKwElse,       else       )
TokKw(TkKwFor,        for        )
TokKw(TkKwBreak,      break      )
TokKw(TkKwContinue,   continue   )
TokKw(TkKwSwitch,     switch     )
TokKw(TkKwFalltrough, falltrouth )
TokKw(TkKwDo,         do         )

TokKw(TkKwType,       type       )
TokKw(TkKwAlias,      alias      )
TokKw(TkKwStruct,     struct     )
TokKw(TkKwEnum,       enum       )
TokKw(TkKwUnion,      union      )
TokKw(TkKwTrait,      trait      )
TokKw(TkKwImpl,       impl       )

/* symbols */
Tok(TkColon           ) /* :   */
Tok(TkSemiColon       ) /* ;   */
Tok(TkDot             ) /* .   */
Tok(TkDotMinus        ) /* .-  */
Tok(TkDotStar         ) /* .*  */
Tok(TkDotAmpersand    ) /* .&  */
Tok(TkDotBang         ) /* .!  */
Tok(TkDotDot          ) /* ..  */
Tok(TkDotDotLArrow    ) /* ..< */
Tok(TkComma           ) /* ,   */
Tok(TkOpenParen       ) /* (   */
Tok(TkCloseParen      ) /* )   */
Tok(TkOpenBracket     ) /* [   */
Tok(TkCloseBracket    ) /* ]   */
Tok(TkOpenBrace       ) /* {   */
Tok(TkCloseBrace      ) /* }   */

Tok(TkPlus            ) /* +   */
Tok(TkMinus           ) /* -   */
Tok(TkSlash           ) /* /   */
Tok(TkStar            ) /* *   */
Tok(TkPercent         ) /* %   */

Tok(TkEq              ) /* =   */
Tok(TkPlusEq          ) /* +=  */
Tok(TkMinusEq         ) /* -=  */
Tok(TkSlashEq         ) /* /=  */
Tok(TkStarEq          ) /* *=  */
Tok(TkPercentEq       ) /* %=  */

Tok(TkLArrow          ) /* <   */
Tok(TkRArrow          ) /* >   */
Tok(TkEqEq            ) /* ==  */
Tok(TkBangEq          ) /* !=  */
Tok(TkLArrowEq        ) /* <=  */
Tok(TkRArrowEq        ) /* >=  */
