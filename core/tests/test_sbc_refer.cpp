#include "fct.h"

#include "AmSipMsg.h"
#include "AmSipHeaders.h"

#include "../../apps/sbc/SBCCallLeg.h"
#include "../../apps/sbc/SBCCallProfile.h"

FCTMF_SUITE_BGN(test_sbc_refer) {

  FCT_TEST_BGN(refer_mode_defaults_to_relay) {
    bool valid = false;
    fct_chk(SBCCallProfile::parseReferMode("", &valid) ==
	    SBCCallProfile::ReferModeRelay);
    fct_chk(valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_mode_supported_values) {
    bool valid = false;
    fct_chk(SBCCallProfile::parseReferMode("relay", &valid) ==
	    SBCCallProfile::ReferModeRelay && valid);
    fct_chk(SBCCallProfile::parseReferMode("local", &valid) ==
	    SBCCallProfile::ReferModeLocal && valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_mode_invalid_falls_back_to_relay) {
    bool valid = true;
    fct_chk(SBCCallProfile::parseReferMode("invalid", &valid) ==
	    SBCCallProfile::ReferModeRelay);
    fct_chk(!valid);
    fct_chk(SBCCallProfile::parseReferMode("reject", &valid) ==
	    SBCCallProfile::ReferModeRelay);
    fct_chk(!valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(relay_policy_has_no_profile_state_or_dump_changes) {
    SBCCallProfile profile;
    profile.setReferPolicy(SBCCallProfile::ReferModeRelay,
			   SBCCallProfile::ReferLocalLegA,
			   "ignored.example.net");
    fct_chk(profile.getReferMode() == SBCCallProfile::ReferModeRelay);
    fct_chk(profile.getReferLocalLeg() == SBCCallProfile::ReferLocalLegB);
    fct_chk(profile.getReferLocalDomain().empty());
    fct_chk(profile.cc_vars.empty());
    fct_chk(profile.print().find("refer_mode:") == string::npos);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_local_leg_defaults_to_bleg) {
    bool valid = false;
    fct_chk(SBCCallProfile::parseReferLocalLeg("", &valid) ==
	    SBCCallProfile::ReferLocalLegB);
    fct_chk(valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_local_leg_supported_values) {
    bool valid = false;
    fct_chk(SBCCallProfile::parseReferLocalLeg("aleg", &valid) ==
	    SBCCallProfile::ReferLocalLegA && valid);
    fct_chk(SBCCallProfile::parseReferLocalLeg("BLEG", &valid) ==
	    SBCCallProfile::ReferLocalLegB && valid);
    fct_chk(SBCCallProfile::parseReferLocalLeg("both", &valid) ==
	    SBCCallProfile::ReferLocalLegBoth && valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_local_leg_invalid_falls_back_to_bleg) {
    bool valid = true;
    fct_chk(SBCCallProfile::parseReferLocalLeg("invalid", &valid) ==
	    SBCCallProfile::ReferLocalLegB);
    fct_chk(!valid);
  } FCT_TEST_END();

  FCT_TEST_BGN(refer_local_leg_restricts_source) {
    SBCCallProfile profile;
    fct_chk(!profile.allowsLocalRefer(true));
    fct_chk(profile.allowsLocalRefer(false));
    profile.setReferPolicy(SBCCallProfile::ReferModeLocal,
			   SBCCallProfile::ReferLocalLegA,
			   "outbound.example.net");
    fct_chk(profile.allowsLocalRefer(true));
    fct_chk(!profile.allowsLocalRefer(false));
    profile.setReferPolicy(SBCCallProfile::ReferModeLocal,
			   SBCCallProfile::ReferLocalLegBoth,
			   "outbound.example.net");
    fct_chk(profile.allowsLocalRefer(true));
    fct_chk(profile.allowsLocalRefer(false));
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_requires_controlled_routing) {
    SBCCallProfile profile;
    fct_chk(!profile.hasLocalReferRouting());
    profile.setReferPolicy(SBCCallProfile::ReferModeLocal,
			   SBCCallProfile::ReferLocalLegB,
			   "outbound.example.net");
    fct_chk(profile.hasLocalReferRouting());
    profile.setReferPolicy(SBCCallProfile::ReferModeLocal,
			   SBCCallProfile::ReferLocalLegB, "bad/domain");
    fct_chk(!profile.hasLocalReferRouting());
  } FCT_TEST_END();

  FCT_TEST_BGN(local_policy_is_explicit_and_visible) {
    SBCCallProfile profile;
    profile.setReferPolicy(SBCCallProfile::ReferModeLocal,
			   SBCCallProfile::ReferLocalLegBoth,
			   "outbound.example.net");
    fct_chk(profile.getReferMode() == SBCCallProfile::ReferModeLocal);
    fct_chk(profile.getReferLocalLeg() == SBCCallProfile::ReferLocalLegBoth);
    fct_chk(profile.getReferLocalDomain() == "outbound.example.net");
    fct_chk(profile.print().find("refer_mode:           local") != string::npos);
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_missing_target) {
    AmSipRequest req;
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(!SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					 party, uri, hdrs, code, reason));
    fct_chk(code == 400);
    fct_chk(reason == "Missing Refer-To");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_invalid_target) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(!SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					 party, uri, hdrs, code, reason));
    fct_chk(code == 400);
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_normalizes_tel_target) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <tel:+390212345678>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					party, uri, hdrs, code, reason));
    fct_chk(uri == "sip:+390212345678@outbound.example.net;user=phone");
    fct_chk(party == "<sip:+390212345678@outbound.example.net;user=phone>");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_normalizes_unbracketed_tel_target) {
    AmSipRequest req;
    req.hdrs = "Refer-To: tel:+390212345678\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					party, uri, hdrs, code, reason));
    fct_chk(uri == "sip:+390212345678@outbound.example.net;user=phone");
    fct_chk(party == "<sip:+390212345678@outbound.example.net;user=phone>");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_normalizes_angle_bracket_sip_target) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:+390212345678@retell.example>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					party, uri, hdrs, code, reason));
    fct_chk(uri == "sip:+390212345678@outbound.example.net;user=phone");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_forbidden_uri_header) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:+390212345678@example.net?X-Destination=elsewhere>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(!SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					 party, uri, hdrs, code, reason));
    fct_chk(code == 403);
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_valid_target_and_replaces) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sips:+390212345678@example.net;user=phone?"
	"Replaces=call-1%3Bto-tag%3Dto%3Bfrom-tag%3Dfrom&Require=replaces>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					party, uri, hdrs, code, reason));
    fct_chk(uri == "sip:+390212345678@outbound.example.net;user=phone");
    fct_chk(getHeader(hdrs, SIP_HDR_REPLACES, true) ==
	    "call-1;to-tag=to;from-tag=from");
    fct_chk(getHeader(hdrs, SIP_HDR_REQUIRE, true) == "replaces");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_rejects_multiple_headers) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:+390212345678@example.net>\r\n"
	"Refer-To: <sip:+390298765432@example.net>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(!SBCCallLeg::parseReferTarget(req, "outbound.example.net",
					 party, uri, hdrs, code, reason));
    fct_chk(code == 400);
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_forces_configured_domain) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:0458271636@attacker.example>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(SBCCallLeg::parseReferTarget(req, "carrier.example.net",
					party, uri, hdrs, code, reason));
    fct_chk(uri == "sip:0458271636@carrier.example.net;user=phone");
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_rejects_non_numeric_user) {
    AmSipRequest req;
    req.hdrs = "Refer-To: <sip:alice@example.net>\r\n";
    string party, uri, hdrs, reason;
    unsigned int code = 0;
    fct_chk(!SBCCallLeg::parseReferTarget(req, "carrier.example.net",
					 party, uri, hdrs, code, reason));
    fct_chk(code == 400);
  } FCT_TEST_END();

  FCT_TEST_BGN(local_refer_builds_reverse_invite_identities) {
    AmSipRequest invite;
    fct_chk(SBCCallLeg::buildLocalReferInvite(
	"<sip:+3904511170261@access.example;user=phone>",
	"<sip:+393453192740@sbc.example;user=phone>",
	"<sip:+390458271636@carrier.example;user=phone>",
	"sip:+390458271636@carrier.example;user=phone", "", invite));
    fct_chk(invite.r_uri ==
	    "sip:+390458271636@carrier.example;user=phone");
    fct_chk(invite.to ==
	    "<sip:+390458271636@carrier.example;user=phone>");
    fct_chk(invite.from ==
	    "<sip:+3904511170261@access.example;user=phone>");
    fct_chk(getHeader(invite.hdrs, "Diversion", true) ==
	    "<sip:+393453192740@sbc.example;user=phone>;"
	    "reason=unconditional;counter=1");
  } FCT_TEST_END();

} FCTMF_SUITE_END();
