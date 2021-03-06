/* Do not edit: automatically built by gen_rec.awk. */
#include <errno.h>
#include "db_config.h"

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <ctype.h>
#include <string.h>
#endif

#include "db_int.h"
#include "db_page.h"
#include "db_dispatch.h"
#include "db_am.h"
#include "txn.h"

int
CDB___crdel_fileopen_log (dbenv, txnid, ret_lsnp, flags, name, mode)
     DB_ENV *dbenv;
     DB_TXN *txnid;
     DB_LSN *ret_lsnp;
     uint32_t flags;
     const DBT *name;
     uint32_t mode;
{
  DBT logrec;
  DB_LSN *lsnp, null_lsn;
  uint32_t zero;
  uint32_t rectype, txn_num;
  int ret;
  uint8_t *bp;

  if (txnid != NULL &&
      TAILQ_FIRST (&txnid->kids) != NULL && CDB___txn_activekids (txnid) != 0)
    return (EPERM);
  rectype = DB_crdel_fileopen;
  txn_num = txnid == NULL ? 0 : txnid->txnid;
  if (txnid == NULL)
  {
    ZERO_LSN (null_lsn);
    lsnp = &null_lsn;
  }
  else
    lsnp = &txnid->last_lsn;
  logrec.size = sizeof (rectype) + sizeof (txn_num) + sizeof (DB_LSN)
    + sizeof (uint32_t) + (name == NULL ? 0 : name->size) + sizeof (mode);
  if ((ret = CDB___os_malloc (logrec.size, NULL, &logrec.data)) != 0)
    return (ret);

  bp = logrec.data;
  memcpy (bp, &rectype, sizeof (rectype));
  bp += sizeof (rectype);
  memcpy (bp, &txn_num, sizeof (txn_num));
  bp += sizeof (txn_num);
  memcpy (bp, lsnp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  if (name == NULL)
  {
    zero = 0;
    memcpy (bp, &zero, sizeof (uint32_t));
    bp += sizeof (uint32_t);
  }
  else
  {
    memcpy (bp, &name->size, sizeof (name->size));
    bp += sizeof (name->size);
    memcpy (bp, name->data, name->size);
    bp += name->size;
  }
  memcpy (bp, &mode, sizeof (mode));
  bp += sizeof (mode);
  DB_ASSERT ((uint32_t) (bp - (uint8_t *) logrec.data) == logrec.size);
  ret = CDB_log_put (dbenv, ret_lsnp, (DBT *) & logrec, flags);
  if (txnid != NULL)
    txnid->last_lsn = *ret_lsnp;
  CDB___os_free (logrec.data, logrec.size);
  return (ret);
}

int
CDB___crdel_fileopen_print (notused1, dbtp, lsnp, notused2, notused3)
     DB_ENV *notused1;
     DBT *dbtp;
     DB_LSN *lsnp;
     int notused2;
     void *notused3;
{
  __crdel_fileopen_args *argp;
  uint32_t i;
  u_int ch;
  int ret;

  i = 0;
  ch = 0;
  notused1 = NULL;
  notused2 = 0;
  notused3 = NULL;

  if ((ret = CDB___crdel_fileopen_read (dbtp->data, &argp)) != 0)
    return (ret);
  printf ("[%lu][%lu]crdel_fileopen: rec: %lu txnid %lx prevlsn [%lu][%lu]\n",
          (u_long) lsnp->file,
          (u_long) lsnp->offset,
          (u_long) argp->type,
          (u_long) argp->txnid->txnid,
          (u_long) argp->prev_lsn.file, (u_long) argp->prev_lsn.offset);
  printf ("\tname: ");
  for (i = 0; i < argp->name.size; i++)
  {
    ch = ((uint8_t *) argp->name.data)[i];
    if (isprint (ch) || ch == 0xa)
      putchar (ch);
    else
      printf ("%#x ", ch);
  }
  printf ("\n");
  printf ("\tmode: %o\n", argp->mode);
  printf ("\n");
  CDB___os_free (argp, 0);
  return (0);
}

int
CDB___crdel_fileopen_read (recbuf, argpp)
     void *recbuf;
     __crdel_fileopen_args **argpp;
{
  __crdel_fileopen_args *argp;
  uint8_t *bp;
  int ret;

  ret = CDB___os_malloc (sizeof (__crdel_fileopen_args) +
                         sizeof (DB_TXN), NULL, &argp);
  if (ret != 0)
    return (ret);
  argp->txnid = (DB_TXN *) & argp[1];
  bp = recbuf;
  memcpy (&argp->type, bp, sizeof (argp->type));
  bp += sizeof (argp->type);
  memcpy (&argp->txnid->txnid, bp, sizeof (argp->txnid->txnid));
  bp += sizeof (argp->txnid->txnid);
  memcpy (&argp->prev_lsn, bp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memset (&argp->name, 0, sizeof (argp->name));
  memcpy (&argp->name.size, bp, sizeof (uint32_t));
  bp += sizeof (uint32_t);
  argp->name.data = bp;
  bp += argp->name.size;
  memcpy (&argp->mode, bp, sizeof (argp->mode));
  bp += sizeof (argp->mode);
  *argpp = argp;
  return (0);
}

int
CDB___crdel_metasub_log (dbenv, txnid, ret_lsnp, flags,
                         fileid, pgno, page, lsn)
     DB_ENV *dbenv;
     DB_TXN *txnid;
     DB_LSN *ret_lsnp;
     uint32_t flags;
     uint32_t fileid;
     db_pgno_t pgno;
     const DBT *page;
     DB_LSN *lsn;
{
  DBT logrec;
  DB_LSN *lsnp, null_lsn;
  uint32_t zero;
  uint32_t rectype, txn_num;
  int ret;
  uint8_t *bp;

  if (txnid != NULL &&
      TAILQ_FIRST (&txnid->kids) != NULL && CDB___txn_activekids (txnid) != 0)
    return (EPERM);
  rectype = DB_crdel_metasub;
  txn_num = txnid == NULL ? 0 : txnid->txnid;
  if (txnid == NULL)
  {
    ZERO_LSN (null_lsn);
    lsnp = &null_lsn;
  }
  else
    lsnp = &txnid->last_lsn;
  logrec.size = sizeof (rectype) + sizeof (txn_num) + sizeof (DB_LSN)
    + sizeof (fileid)
    + sizeof (pgno)
    + sizeof (uint32_t) + (page == NULL ? 0 : page->size) + sizeof (*lsn);
  if ((ret = CDB___os_malloc (logrec.size, NULL, &logrec.data)) != 0)
    return (ret);

  bp = logrec.data;
  memcpy (bp, &rectype, sizeof (rectype));
  bp += sizeof (rectype);
  memcpy (bp, &txn_num, sizeof (txn_num));
  bp += sizeof (txn_num);
  memcpy (bp, lsnp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memcpy (bp, &fileid, sizeof (fileid));
  bp += sizeof (fileid);
  memcpy (bp, &pgno, sizeof (pgno));
  bp += sizeof (pgno);
  if (page == NULL)
  {
    zero = 0;
    memcpy (bp, &zero, sizeof (uint32_t));
    bp += sizeof (uint32_t);
  }
  else
  {
    memcpy (bp, &page->size, sizeof (page->size));
    bp += sizeof (page->size);
    memcpy (bp, page->data, page->size);
    bp += page->size;
  }
  if (lsn != NULL)
    memcpy (bp, lsn, sizeof (*lsn));
  else
    memset (bp, 0, sizeof (*lsn));
  bp += sizeof (*lsn);
  DB_ASSERT ((uint32_t) (bp - (uint8_t *) logrec.data) == logrec.size);
  ret = CDB_log_put (dbenv, ret_lsnp, (DBT *) & logrec, flags);
  if (txnid != NULL)
    txnid->last_lsn = *ret_lsnp;
  CDB___os_free (logrec.data, logrec.size);
  return (ret);
}

int
CDB___crdel_metasub_print (notused1, dbtp, lsnp, notused2, notused3)
     DB_ENV *notused1;
     DBT *dbtp;
     DB_LSN *lsnp;
     int notused2;
     void *notused3;
{
  __crdel_metasub_args *argp;
  uint32_t i;
  u_int ch;
  int ret;

  i = 0;
  ch = 0;
  notused1 = NULL;
  notused2 = 0;
  notused3 = NULL;

  if ((ret = CDB___crdel_metasub_read (dbtp->data, &argp)) != 0)
    return (ret);
  printf ("[%lu][%lu]crdel_metasub: rec: %lu txnid %lx prevlsn [%lu][%lu]\n",
          (u_long) lsnp->file,
          (u_long) lsnp->offset,
          (u_long) argp->type,
          (u_long) argp->txnid->txnid,
          (u_long) argp->prev_lsn.file, (u_long) argp->prev_lsn.offset);
  printf ("\tfileid: %lu\n", (u_long) argp->fileid);
  printf ("\tpgno: %d\n", argp->pgno);
  printf ("\tpage: ");
  for (i = 0; i < argp->page.size; i++)
  {
    ch = ((uint8_t *) argp->page.data)[i];
    if (isprint (ch) || ch == 0xa)
      putchar (ch);
    else
      printf ("%#x ", ch);
  }
  printf ("\n");
  printf ("\tlsn: [%lu][%lu]\n",
          (u_long) argp->lsn.file, (u_long) argp->lsn.offset);
  printf ("\n");
  CDB___os_free (argp, 0);
  return (0);
}

int
CDB___crdel_metasub_read (recbuf, argpp)
     void *recbuf;
     __crdel_metasub_args **argpp;
{
  __crdel_metasub_args *argp;
  uint8_t *bp;
  int ret;

  ret = CDB___os_malloc (sizeof (__crdel_metasub_args) +
                         sizeof (DB_TXN), NULL, &argp);
  if (ret != 0)
    return (ret);
  argp->txnid = (DB_TXN *) & argp[1];
  bp = recbuf;
  memcpy (&argp->type, bp, sizeof (argp->type));
  bp += sizeof (argp->type);
  memcpy (&argp->txnid->txnid, bp, sizeof (argp->txnid->txnid));
  bp += sizeof (argp->txnid->txnid);
  memcpy (&argp->prev_lsn, bp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memcpy (&argp->fileid, bp, sizeof (argp->fileid));
  bp += sizeof (argp->fileid);
  memcpy (&argp->pgno, bp, sizeof (argp->pgno));
  bp += sizeof (argp->pgno);
  memset (&argp->page, 0, sizeof (argp->page));
  memcpy (&argp->page.size, bp, sizeof (uint32_t));
  bp += sizeof (uint32_t);
  argp->page.data = bp;
  bp += argp->page.size;
  memcpy (&argp->lsn, bp, sizeof (argp->lsn));
  bp += sizeof (argp->lsn);
  *argpp = argp;
  return (0);
}

int
CDB___crdel_metapage_log (dbenv, txnid, ret_lsnp, flags,
                          fileid, name, pgno, page)
     DB_ENV *dbenv;
     DB_TXN *txnid;
     DB_LSN *ret_lsnp;
     uint32_t flags;
     uint32_t fileid;
     const DBT *name;
     db_pgno_t pgno;
     const DBT *page;
{
  DBT logrec;
  DB_LSN *lsnp, null_lsn;
  uint32_t zero;
  uint32_t rectype, txn_num;
  int ret;
  uint8_t *bp;

  if (txnid != NULL &&
      TAILQ_FIRST (&txnid->kids) != NULL && CDB___txn_activekids (txnid) != 0)
    return (EPERM);
  rectype = DB_crdel_metapage;
  txn_num = txnid == NULL ? 0 : txnid->txnid;
  if (txnid == NULL)
  {
    ZERO_LSN (null_lsn);
    lsnp = &null_lsn;
  }
  else
    lsnp = &txnid->last_lsn;
  logrec.size = sizeof (rectype) + sizeof (txn_num) + sizeof (DB_LSN)
    + sizeof (fileid)
    + sizeof (uint32_t) + (name == NULL ? 0 : name->size)
    + sizeof (pgno) + sizeof (uint32_t) + (page == NULL ? 0 : page->size);
  if ((ret = CDB___os_malloc (logrec.size, NULL, &logrec.data)) != 0)
    return (ret);

  bp = logrec.data;
  memcpy (bp, &rectype, sizeof (rectype));
  bp += sizeof (rectype);
  memcpy (bp, &txn_num, sizeof (txn_num));
  bp += sizeof (txn_num);
  memcpy (bp, lsnp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memcpy (bp, &fileid, sizeof (fileid));
  bp += sizeof (fileid);
  if (name == NULL)
  {
    zero = 0;
    memcpy (bp, &zero, sizeof (uint32_t));
    bp += sizeof (uint32_t);
  }
  else
  {
    memcpy (bp, &name->size, sizeof (name->size));
    bp += sizeof (name->size);
    memcpy (bp, name->data, name->size);
    bp += name->size;
  }
  memcpy (bp, &pgno, sizeof (pgno));
  bp += sizeof (pgno);
  if (page == NULL)
  {
    zero = 0;
    memcpy (bp, &zero, sizeof (uint32_t));
    bp += sizeof (uint32_t);
  }
  else
  {
    memcpy (bp, &page->size, sizeof (page->size));
    bp += sizeof (page->size);
    memcpy (bp, page->data, page->size);
    bp += page->size;
  }
  DB_ASSERT ((uint32_t) (bp - (uint8_t *) logrec.data) == logrec.size);
  ret = CDB_log_put (dbenv, ret_lsnp, (DBT *) & logrec, flags);
  if (txnid != NULL)
    txnid->last_lsn = *ret_lsnp;
  CDB___os_free (logrec.data, logrec.size);
  return (ret);
}

int
CDB___crdel_metapage_print (notused1, dbtp, lsnp, notused2, notused3)
     DB_ENV *notused1;
     DBT *dbtp;
     DB_LSN *lsnp;
     int notused2;
     void *notused3;
{
  __crdel_metapage_args *argp;
  uint32_t i;
  u_int ch;
  int ret;

  i = 0;
  ch = 0;
  notused1 = NULL;
  notused2 = 0;
  notused3 = NULL;

  if ((ret = CDB___crdel_metapage_read (dbtp->data, &argp)) != 0)
    return (ret);
  printf ("[%lu][%lu]crdel_metapage: rec: %lu txnid %lx prevlsn [%lu][%lu]\n",
          (u_long) lsnp->file,
          (u_long) lsnp->offset,
          (u_long) argp->type,
          (u_long) argp->txnid->txnid,
          (u_long) argp->prev_lsn.file, (u_long) argp->prev_lsn.offset);
  printf ("\tfileid: %lu\n", (u_long) argp->fileid);
  printf ("\tname: ");
  for (i = 0; i < argp->name.size; i++)
  {
    ch = ((uint8_t *) argp->name.data)[i];
    if (isprint (ch) || ch == 0xa)
      putchar (ch);
    else
      printf ("%#x ", ch);
  }
  printf ("\n");
  printf ("\tpgno: %d\n", argp->pgno);
  printf ("\tpage: ");
  for (i = 0; i < argp->page.size; i++)
  {
    ch = ((uint8_t *) argp->page.data)[i];
    if (isprint (ch) || ch == 0xa)
      putchar (ch);
    else
      printf ("%#x ", ch);
  }
  printf ("\n");
  printf ("\n");
  CDB___os_free (argp, 0);
  return (0);
}

int
CDB___crdel_metapage_read (recbuf, argpp)
     void *recbuf;
     __crdel_metapage_args **argpp;
{
  __crdel_metapage_args *argp;
  uint8_t *bp;
  int ret;

  ret = CDB___os_malloc (sizeof (__crdel_metapage_args) +
                         sizeof (DB_TXN), NULL, &argp);
  if (ret != 0)
    return (ret);
  argp->txnid = (DB_TXN *) & argp[1];
  bp = recbuf;
  memcpy (&argp->type, bp, sizeof (argp->type));
  bp += sizeof (argp->type);
  memcpy (&argp->txnid->txnid, bp, sizeof (argp->txnid->txnid));
  bp += sizeof (argp->txnid->txnid);
  memcpy (&argp->prev_lsn, bp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memcpy (&argp->fileid, bp, sizeof (argp->fileid));
  bp += sizeof (argp->fileid);
  memset (&argp->name, 0, sizeof (argp->name));
  memcpy (&argp->name.size, bp, sizeof (uint32_t));
  bp += sizeof (uint32_t);
  argp->name.data = bp;
  bp += argp->name.size;
  memcpy (&argp->pgno, bp, sizeof (argp->pgno));
  bp += sizeof (argp->pgno);
  memset (&argp->page, 0, sizeof (argp->page));
  memcpy (&argp->page.size, bp, sizeof (uint32_t));
  bp += sizeof (uint32_t);
  argp->page.data = bp;
  bp += argp->page.size;
  *argpp = argp;
  return (0);
}

int
CDB___crdel_delete_log (dbenv, txnid, ret_lsnp, flags, name)
     DB_ENV *dbenv;
     DB_TXN *txnid;
     DB_LSN *ret_lsnp;
     uint32_t flags;
     const DBT *name;
{
  DBT logrec;
  DB_LSN *lsnp, null_lsn;
  uint32_t zero;
  uint32_t rectype, txn_num;
  int ret;
  uint8_t *bp;

  if (txnid != NULL &&
      TAILQ_FIRST (&txnid->kids) != NULL && CDB___txn_activekids (txnid) != 0)
    return (EPERM);
  rectype = DB_crdel_delete;
  txn_num = txnid == NULL ? 0 : txnid->txnid;
  if (txnid == NULL)
  {
    ZERO_LSN (null_lsn);
    lsnp = &null_lsn;
  }
  else
    lsnp = &txnid->last_lsn;
  logrec.size = sizeof (rectype) + sizeof (txn_num) + sizeof (DB_LSN)
    + sizeof (uint32_t) + (name == NULL ? 0 : name->size);
  if ((ret = CDB___os_malloc (logrec.size, NULL, &logrec.data)) != 0)
    return (ret);

  bp = logrec.data;
  memcpy (bp, &rectype, sizeof (rectype));
  bp += sizeof (rectype);
  memcpy (bp, &txn_num, sizeof (txn_num));
  bp += sizeof (txn_num);
  memcpy (bp, lsnp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  if (name == NULL)
  {
    zero = 0;
    memcpy (bp, &zero, sizeof (uint32_t));
    bp += sizeof (uint32_t);
  }
  else
  {
    memcpy (bp, &name->size, sizeof (name->size));
    bp += sizeof (name->size);
    memcpy (bp, name->data, name->size);
    bp += name->size;
  }
  DB_ASSERT ((uint32_t) (bp - (uint8_t *) logrec.data) == logrec.size);
  ret = CDB_log_put (dbenv, ret_lsnp, (DBT *) & logrec, flags);
  if (txnid != NULL)
    txnid->last_lsn = *ret_lsnp;
  CDB___os_free (logrec.data, logrec.size);
  return (ret);
}

int
CDB___crdel_delete_print (notused1, dbtp, lsnp, notused2, notused3)
     DB_ENV *notused1;
     DBT *dbtp;
     DB_LSN *lsnp;
     int notused2;
     void *notused3;
{
  __crdel_delete_args *argp;
  uint32_t i;
  u_int ch;
  int ret;

  i = 0;
  ch = 0;
  notused1 = NULL;
  notused2 = 0;
  notused3 = NULL;

  if ((ret = CDB___crdel_delete_read (dbtp->data, &argp)) != 0)
    return (ret);
  printf ("[%lu][%lu]crdel_delete: rec: %lu txnid %lx prevlsn [%lu][%lu]\n",
          (u_long) lsnp->file,
          (u_long) lsnp->offset,
          (u_long) argp->type,
          (u_long) argp->txnid->txnid,
          (u_long) argp->prev_lsn.file, (u_long) argp->prev_lsn.offset);
  printf ("\tname: ");
  for (i = 0; i < argp->name.size; i++)
  {
    ch = ((uint8_t *) argp->name.data)[i];
    if (isprint (ch) || ch == 0xa)
      putchar (ch);
    else
      printf ("%#x ", ch);
  }
  printf ("\n");
  printf ("\n");
  CDB___os_free (argp, 0);
  return (0);
}

int
CDB___crdel_delete_read (recbuf, argpp)
     void *recbuf;
     __crdel_delete_args **argpp;
{
  __crdel_delete_args *argp;
  uint8_t *bp;
  int ret;

  ret = CDB___os_malloc (sizeof (__crdel_delete_args) +
                         sizeof (DB_TXN), NULL, &argp);
  if (ret != 0)
    return (ret);
  argp->txnid = (DB_TXN *) & argp[1];
  bp = recbuf;
  memcpy (&argp->type, bp, sizeof (argp->type));
  bp += sizeof (argp->type);
  memcpy (&argp->txnid->txnid, bp, sizeof (argp->txnid->txnid));
  bp += sizeof (argp->txnid->txnid);
  memcpy (&argp->prev_lsn, bp, sizeof (DB_LSN));
  bp += sizeof (DB_LSN);
  memset (&argp->name, 0, sizeof (argp->name));
  memcpy (&argp->name.size, bp, sizeof (uint32_t));
  bp += sizeof (uint32_t);
  argp->name.data = bp;
  bp += argp->name.size;
  *argpp = argp;
  return (0);
}

int
CDB___crdel_init_print (dbenv)
     DB_ENV *dbenv;
{
  int ret;

  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_fileopen_print,
                                    DB_crdel_fileopen)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_metasub_print,
                                    DB_crdel_metasub)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_metapage_print,
                                    DB_crdel_metapage)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_delete_print,
                                    DB_crdel_delete)) != 0)
    return (ret);
  return (0);
}

/*
 * PUBLIC: int CDB___crdel_init_recover __P((DB_ENV *));
 */
int
CDB___crdel_init_recover (dbenv)
     DB_ENV *dbenv;
{
  int ret;

  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_fileopen_recover,
                                    DB_crdel_fileopen)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_metasub_recover,
                                    DB_crdel_metasub)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_metapage_recover,
                                    DB_crdel_metapage)) != 0)
    return (ret);
  if ((ret = CDB___db_add_recovery (dbenv,
                                    CDB___crdel_delete_recover,
                                    DB_crdel_delete)) != 0)
    return (ret);
  return (0);
}
