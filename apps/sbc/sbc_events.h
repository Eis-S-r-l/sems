#ifndef __SBC_EVENTS_H
#define __SBC_EVENTS_H

#include "CallLeg.h"

enum {
  /** This ID should be used by all CC modules that produce B2B events. Because
   * CC modules are developped indepenedently it is not possible to synchronize
   * their B2B event IDs so only this one should be used. */
  CCB2BEventId = LAST_B2B_CALL_LEG_EVENT_ID + 1,
  LocalReferStartEventId,
  LocalReferCreatedEventId,
  LocalReferResultEventId,
  LocalReferAbortEventId
};

struct LocalReferStartEvent: public B2BEvent
{
  string transferor_id;
  unsigned int refer_cseq;
  string target_party;
  string target_uri;
  string invite_hdrs;

  LocalReferStartEvent(const string& _transferor_id,
		       unsigned int _refer_cseq,
		       const string& _target_party,
		       const string& _target_uri,
		       const string& _invite_hdrs)
    : B2BEvent(LocalReferStartEventId),
      transferor_id(_transferor_id), refer_cseq(_refer_cseq),
      target_party(_target_party), target_uri(_target_uri),
      invite_hdrs(_invite_hdrs)
  { }
};

struct LocalReferCreatedEvent: public B2BEvent
{
  unsigned int refer_cseq;
  bool created;
  string new_leg_id;
  unsigned int code;
  string reason;

  LocalReferCreatedEvent(unsigned int _refer_cseq, bool _created,
			 const string& _new_leg_id,
			 unsigned int _code = 500,
			 const string& _reason = "Server Internal Error")
    : B2BEvent(LocalReferCreatedEventId), refer_cseq(_refer_cseq),
      created(_created), new_leg_id(_new_leg_id), code(_code), reason(_reason)
  { }
};

struct LocalReferResultEvent: public B2BEvent
{
  unsigned int refer_cseq;
  unsigned int code;
  string reason;
  bool final;

  LocalReferResultEvent(unsigned int _refer_cseq, unsigned int _code,
			const string& _reason, bool _final)
    : B2BEvent(LocalReferResultEventId), refer_cseq(_refer_cseq),
      code(_code), reason(_reason), final(_final)
  { }
};

struct LocalReferAbortEvent: public B2BEvent
{
  string transferor_id;
  unsigned int refer_cseq;

  LocalReferAbortEvent(const string& _transferor_id, unsigned int _refer_cseq)
    : B2BEvent(LocalReferAbortEventId), transferor_id(_transferor_id),
      refer_cseq(_refer_cseq)
  { }
};

#endif
