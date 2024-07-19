
CREATE OR REPLACE FUNCTION public.no_crash_test(func TEXT, params TEXT[], subs TEXT[],
                                                error_messages TEXT[] DEFAULT ARRAY[]::TEXT[],
                                                non_empty_args INTEGER[] DEFAULT ARRAY[0]::INTEGER[])
RETURNS SETOF TEXT AS
$BODY$
DECLARE
mp TEXT[];
q1 TEXT;
q TEXT;
separator TEXT;
BEGIN
    FOR i IN 0..array_length(params, 1) LOOP
        separator = ' ';
        mp := params;
        IF i != 0 THEN
            mp[i] = subs[i];
        END IF;

        q1 := 'SELECT * FROM ' || $1 || ' (';

        FOR i IN 1..array_length(mp, 1) LOOP
            q1 := q1 || separator || mp[i];
            separator :=',';
        END LOOP;

        q1 := q1 || ')';

        -- RAISE WARNING '%', q1;


        IF array_length(error_messages, 1) >= i AND error_messages[i] != '' THEN
          RETURN query SELECT throws_ok(q1, error_messages[i]);
        ELSE
          RETURN query SELECT * FROM lives_ok(q1, 'lives_ok:' || q1);
          IF i = ANY(non_empty_args) THEN
              RETURN query SELECT * FROM lives_ok(q1, 'lives_ok: ' || q1);
          ELSE
              RETURN query SELECT * FROM is_empty(q1, 'is_empty: ' || q1);
          END IF;
        END IF;

    END LOOP;

END
$BODY$
LANGUAGE plpgsql VOLATILE;
