#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"
#include "data-mov/push.h"
#include "data-mov/pop.h"
#include "data-mov/cltd.h"
#include "data-mov/leave.h"
#include "data-mov/movext.h"
#include "data-mov/movzb.h"
#include "data-mov/movzw.h"
#include "data-mov/movsx.h"
#include "data-mov/lgdt.h"

#include "call-jump-ret/call.h"
#include "call-jump-ret/je.h"
#include "call-jump-ret/ret.h"
#include "call-jump-ret/jmp.h"
#include "call-jump-ret/jbe.h"
#include "call-jump-ret/jne.h"
#include "call-jump-ret/jle.h"
#include "call-jump-ret/jg.h"
#include "call-jump-ret/jl.h"
#include "call-jump-ret/jge.h"
#include "call-jump-ret/ja.h"
#include "call-jump-ret/js.h"
#include "call-jump-ret/jns.h"
#include "call-jump-ret/jb.h"
#include "call-jump-ret/jae.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
#include "arith/cmp.h"
#include "arith/add.h"
#include "arith/adc.h"
#include "arith/sub.h"
#include "arith/sbb.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "logic/test.h"
#include "logic/setne.h"
#include "logic/std.h"
#include "logic/cld.h"

#include "string/rep.h"
#include "string/movs.h"
#include "string/lods.h"
#include "string/scas.h"
#include "string/stos.h"

#include "misc/misc.h"

#include "special/special.h"
