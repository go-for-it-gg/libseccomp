/**
 * Seccomp Library test program
 *
 * Author: Paul Moore <pmoore@redhat.com>, Vitaly Shukela <vi0oss@gmail.com>
 */

/*
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses>.
 */

#include <unistd.h>

#include <seccomp.h>

#include "util.h"

int main(int argc, char *argv[])
{
	int rc;
	struct util_options opts;
	scmp_filter_ctx ctx;
	struct scmp_arg_cmp arg_cmp;

	rc = util_getopt(argc, argv, &opts);
	if (rc < 0)
		goto out;

	ctx = seccomp_init(SCMP_ACT_KILL);
	if (ctx == NULL)
		goto out;

	arg_cmp = SCMP_A0(SCMP_CMP_EQ, STDIN_FILENO);
	rc = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ALLOW,
					  SCMP_SYS(read), 1, &arg_cmp);
	if (rc != 0)
		goto out;

	arg_cmp = SCMP_A0(SCMP_CMP_EQ, STDOUT_FILENO);
	rc = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ALLOW,
					  SCMP_SYS(write), 1, &arg_cmp);
	if (rc != 0)
		goto out;

	arg_cmp = SCMP_A0(SCMP_CMP_EQ, STDERR_FILENO);
	rc = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ALLOW,
					  SCMP_SYS(write), 1, &arg_cmp);
	if (rc != 0)
		goto out;

	rc = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ALLOW,
					  SCMP_SYS(close), 0, NULL);
	if (rc != 0)
		goto out;

	rc = seccomp_rule_add_exact_array(ctx, SCMP_ACT_ALLOW,
					  SCMP_SYS(rt_sigreturn), 0, NULL);
	if (rc != 0)
		goto out;

	rc = util_filter_output(&opts, ctx);
	if (rc)
		goto out;

out:
	seccomp_release(ctx);
	return (rc < 0 ? -rc : rc);
}
