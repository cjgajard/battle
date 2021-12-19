#pragma once
#include "config.hpp"
#include "geometry.hpp"

struct command {
	// Next should calculate and save a delta in an private property in
	// order to to Apply it after.
	virtual bool Next (void) = 0;
	virtual void Apply (void) = 0;
	virtual void Halt (void) = 0;
	virtual void End (void) { Halt(); }
	virtual ~command (void) = default;
};

struct move : public command {
	unitid_t uid;
	struct point target;
	struct point mv;
	angle_t a;

	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	move (void) = default;
	move (unitid_t uid, struct point tar);
};

struct attack : public command {
	unitid_t uid;
	unitid_t target;
	unsigned int lastatktime;
	int state;
	bool outofrange;
	struct move mv;

	bool Next (void) override;
	void Apply (void) override;
	void Halt (void) override;
	attack (void) = default;
	attack (unitid_t uid, unitid_t target);
};
