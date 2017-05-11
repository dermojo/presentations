/**
 * Defines all functions that check whether something is allowed.
 */

#pragma once

#include "signatures.hpp" // for SignatureStatus

/**
 * Checks whether interactive access is allowed. The user may be queried for a password.
 * @param[in] isInteractive     whether STDIN is interactive
 * @return true if access is granted
 */
bool checkInteractiveAccess(bool isInteractive);

/**
 * Checks a file's signature (detached or for standalone scripts).
 * @param[in] file      file path
 * @return the check status
 */
SignatureStatus checkFileSignature(const wchar_t* file);
