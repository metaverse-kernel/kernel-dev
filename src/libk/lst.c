#include <libk/lst.h>

#include <kernel/kernel.h>
#include <kernel/memm.h>

#include <libk/math.h>

lst_iterator_t *lst_new(usize start, usize end)
{
    lst_iterator_t *lst = memm_kernel_allocate(sizeof(lst_iterator_t), 0);
    lst->line.left = start;
    lst->line.right = end;
    lst->next = nullptr;
    return lst;
}

lst_iterator_t *lst_next(lst_iterator_t *iterator)
{
    return iterator->next;
}

static lst_line_t get_overlap(lst_line_t *l1, lst_line_t *l2)
{
    lst_line_t l;
    if (l1->right < l2->left || l2->right < l1->left)
    {
        l.left = l.right = 0;
        return l;
    }
    if (l1->left < l2->left)
    {
        if (l1->right < l2->right)
        {
            l.left = l2->left;
            l.right = l1->right;
            return l;
        }
        else
        {
            l.left = l2->left;
            l.right = l2->right;
            return l;
        }
    }
    else
    {
        if (l1->right < l2->right)
        {
            l.left = l1->left;
            l.right = l1->right;
            return l;
        }
        else
        {
            l.left = l1->left;
            l.right = l2->right;
            return l;
        }
    }
}

/*
线的减法

只处理有一条边重合的情况
 */
static void substract_line(lst_line_t *line, lst_line_t *substractor)
{
    if (line->left == substractor->left && line->right > substractor->right)
    {
        line->left = substractor->right;
    }
    else if (line->right == substractor->right && line->left < substractor->left)
    {
        line->right = substractor->left;
    }
}

#define line_is_equal(a, b) ((a).left == (b).left && (a).right == (b).right)
#define line_is_zero(l) ((l).left == (l).right)

bool lst_remove(lst_iterator_t *lst, usize left, usize right, bool force)
{
    while (lst != nullptr)
    {
        if (left > lst->line.left && right < lst->line.right)
        {
            lst_iterator_t *new_node = lst_new(right, lst->line.right);
            lst->line.right = left;

            new_node->next = lst->next;
            lst->next = new_node;
            return true;
        }
        else
        {
            lst_line_t line = {
                .left = left,
                .right = right,
            };
            lst_line_t ol = get_overlap(&line, &lst->line);
            substract_line(&line, &ol);
            if (!line_is_zero(line) && !force)
                return false;
            substract_line(&lst->line, &ol);
            return true;
        }
        lst = lst_next(lst);
    }
    return force;
}

bool lst_add(lst_iterator_t *lst, usize left, usize right, bool force)
{
    lst_iterator_t *last = nullptr;
    while (lst != nullptr)
    {
        if (left < lst->line.left)
        {
            lst_line_t line = {
                .left = left,
                .right = right,
            };
            lst_line_t ol = get_overlap(&line, &lst->line);
            if (!force && !line_is_zero(ol))
                return false;
            if (right >= lst->line.right)
                lst->line.left = left;
            else
            {
                lst_iterator_t *new_node = memm_kernel_allocate(sizeof(lst_iterator_t));
                new_node->line = line;
                new_node->next = lst;
                if (last != nullptr)
                    last->next = new_node;
                return true;
            }
        }
        else if (left <= lst->line.right)
        {
            if (!force && left < lst->line.right)
                return false;
            if (right <= lst->line.right)
            {
                return true;
            }
            else if (right < lst->next->line.left)
            {
                lst->line.right = right;
                return true;
            }
            else
            {
                if (!force && right >= lst->next->line.left)
                    return false;
                lst_iterator_t *tmpnode = lst->next;
                lst_iterator_t tmplast = *lst;
                while (tmpnode != nullptr)
                {
                    if (right < tmpnode->line.left)
                    {
                        lst->line.right = tmplast.line.right;
                        lst->next = tmpnode;
                        return true;
                    }
                    tmplast = *tmpnode;
                    lst_iterator_t *t = lst_next(tmpnode);
                    memm_free(tmpnode);
                    tmpnode = t;
                }
                lst->line.right = max(tmplast.line.right, right);
                lst->next = nullptr;
                return true;
            }
        }
        last = lst;
        lst = lst_next(lst);
    }
}
