#include <stdio.h>
#include <string.h>

#include "aurmodel.h"
#include "aurtexman.h"
#include "aurString.h"
#include "aurbehavior.h"

char g_szNextStringName[256] = "string0";

// Dead code: not in the PC executable
StringGob::StringGob(int nMaxLength)
    : Gob(g_szNextStringName), m_sCaret("["), m_nCaretLinePos(0), m_nCaretColumnPos(0), m_fCaretTime(0.0f)
{
}

// Dead code: not in the PC executable
StringGob::~StringGob()
{
}

// Dead code: not in the PC executable
CAurString* NewCAurString(int nMaxLength)
{
    return NULL;
}

// Dead code: not in the PC executable
void CAurString::DieWithoutParent()
{
}
