/* dnsmasq is Copyright (c) 2000-2021 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
     
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "dnsmasq.h"

#if defined (HAVE_NFTSET) && defined (HAVE_LINUX_NETWORK)

#include <nftables/libnftables.h>

#include <string.h>
#include <arpa/inet.h>

static struct nft_ctx *ctx = NULL;
static const char *cmd_add = "add element %s { %s }";
static const char *cmd_del = "delete element %s { %s }";

#define CMD_BUFFER_SIZE 2048
static char cmd_buf[CMD_BUFFER_SIZE];
static char addr_buf[ADDRSTRLEN + 1];

void nftset_init()
{
  ctx = nft_ctx_new(NFT_CTX_DEFAULT);
  if (ctx == NULL)
    die(_("failed to create nftset context"), NULL, EC_MISC);

  // disable libnftables output
  nft_ctx_buffer_error(ctx);
}

int add_to_nftset(const char *setname, const union all_addr *ipaddr, int flags, int remove)
{
  const char *cmd = remove ? cmd_del : cmd_add;
  int ret, af = (flags & F_IPV4) ? AF_INET : AF_INET6;

  inet_ntop(af, ipaddr, addr_buf, ADDRSTRLEN);
  snprintf(cmd_buf, CMD_BUFFER_SIZE, cmd, setname, addr_buf);

  ret = nft_run_cmd_from_buffer(ctx, cmd_buf);
  nft_ctx_get_error_buffer(ctx);
  return ret;
}

#endif
