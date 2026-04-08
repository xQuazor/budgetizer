#include "LicenseValidator.h"
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>

//==============================================================================
static juce::PropertiesFile::Options getPropsOptions()
{
    juce::PropertiesFile::Options opts;
    opts.applicationName        = "Degradizer";
    opts.filenameSuffix         = ".props";
    opts.osxLibrarySubFolder    = "Application Support";
    return opts;
}

//==============================================================================
bool LicenseValidator::verifySignature(const juce::String& data, const juce::String& sigHex)
{
    juce::RSAKey pubKey(PUBLIC_KEY);
    if (!pubKey.isValid()) return false;

    // Hash the license data with SHA-256.
    // BigInteger::loadFromMemoryBlock treats byte 0 as the LSB,
    // so Python must use int.from_bytes(digest, 'little') when signing.
    const auto utf8 = data.toUTF8();
    juce::SHA256 sha(utf8, data.getNumBytesAsUTF8());
    juce::BigInteger hashInt;
    hashInt.loadFromMemoryBlock(sha.getRawData());

    // Decode signature and apply RSA public key (sig^e mod n).
    juce::BigInteger sig;
    sig.parseString(sigHex, 16);
    pubKey.applyToValue(sig);

    return sig == hashInt;
}

//==============================================================================
LicenseValidator::License LicenseValidator::validate(const juce::String& key)
{
    License result;

    // Expected format:  base64(licenseData):hexSignature
    const int sep = key.lastIndexOfChar(':');
    if (sep < 0) return result;

    const auto b64    = key.substring(0, sep);
    const auto sigHex = key.substring(sep + 1);

    juce::MemoryOutputStream decoded;
    if (!juce::Base64::convertFromBase64(decoded, b64)) return result;

    const auto data = juce::String::fromUTF8(
        static_cast<const char*>(decoded.getData()),
        static_cast<int>(decoded.getDataSize()));

    if (!verifySignature(data, sigHex)) return result;

    // Parse "email=x;product=y"
    for (const auto& token : juce::StringArray::fromTokens(data, ";", ""))
    {
        const int eq = token.indexOfChar('=');
        if (eq < 0) continue;
        const auto k = token.substring(0, eq).trim();
        const auto v = token.substring(eq + 1).trim();
        if (k == "email")   result.email = v;
        if (k == "product" && v != "Degradizer") return {};
    }

    result.valid = result.email.isNotEmpty();
    return result;
}

//==============================================================================
LicenseValidator::License LicenseValidator::checkSaved()
{
    juce::PropertiesFile props(getPropsOptions());
    const auto key = props.getValue("licenseKey");
    if (key.isEmpty()) return {};
    return validate(key);
}

void LicenseValidator::saveKey(const juce::String& key)
{
    juce::PropertiesFile props(getPropsOptions());
    props.setValue("licenseKey", key);
    props.saveIfNeeded();
}
