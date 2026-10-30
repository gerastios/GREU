#include "Language.h"
#include <QObject>
#include <QString>
#include <unordered_map>
#include <vector>

#define LANG2(N, C2) addLanguage(#C2, QObject::tr(N))
#define LANG3(N, C2, C1) addLanguage(#C2, QObject::tr(N), #C1)
#define LANG4(N, C21, C22, C1) addLanguage(#C21, QObject::tr(N), #C1, #C22)

using namespace std;

namespace {
using namespace greu::core;
unordered_map<QString, Language const *const> codeMap;
} // namespace

namespace greu::core {

struct LanguageFactory_ {
    vector<unique_ptr<Language>> languages;

    static LanguageFactory_ *instance() {
        static LanguageFactory_ i;
        return &i;
    }

    void addLanguage(QString const &code2,
                     QString const &name,
                     QString const &code1 = "",
                     QString const &code2_2 = "") {
        auto l = unique_ptr<Language>(new Language(code2, code2_2, code1, name));
        codeMap.insert({code2, l.get()});
        if (!code1.isEmpty())
            codeMap.insert({code1, l.get()});
        if (!code2_2.isEmpty())
            codeMap.insert({code2_2, l.get()});
        codeMap.insert({name, l.get()});
        languages.push_back(std::move(l));
    }

    LanguageFactory_() {
        LANG3("Afar", aar, aa);
        LANG3("Afar", aar, aa);
        LANG3("Abkhazian", abk, ab);
        LANG2("Achinese", ace);
        LANG2("Acoli", ach);
        LANG2("Adangme", ada);
        LANG2("Adygei", ady);
        LANG2("Adyghe", ady);
        LANG2("Afro-Asiatic languages", afa);
        LANG2("Afrihili", afh);
        LANG3("Afrikaans", afr, af);
        LANG2("Ainu", ain);
        LANG3("Akan", aka, ak);
        LANG2("Akkadian", akk);
        LANG4("Albanian", alb, sqi, sq);
        LANG2("Aleut", ale);
        LANG2("Algonquian languages", alg);
        LANG2("Southern Altai", alt);
        LANG3("Amharic", amh, am);
        LANG2("English, Old (ca.450-1100)", ang);
        LANG2("Angika", anp);
        LANG2("Apache languages", apa);
        LANG3("Arabic", ara, ar);
        LANG2("Imperial Aramaic (700-300 BCE)", arc);
        LANG3("Aragonese", arg, an);
        LANG4("Armenian", arm, hye, hy);
        LANG2("Mapuche", arn);
        LANG2("Mapudungun", arn);
        LANG2("Arapaho", arp);
        LANG2("Artificial languages", art);
        LANG2("Arawak", arw);
        LANG3("Assamese", asm, as);
        LANG2("Asturian", ast);
        LANG2("Athapascan languages", ath);
        LANG2("Australian languages", aus);
        LANG3("Avaric", ava, av);
        LANG3("Avestan", ave, ae);
        LANG2("Awadhi", awa);
        LANG3("Aymara", aym, ay);
        LANG3("Azerbaijani", aze, az);
        LANG2("Banda languages", bad);
        LANG2("Bamileke languages", bai);
        LANG3("Bashkir", bak, ba);
        LANG2("Baluchi", bal);
        LANG3("Bambara", bam, bm);
        LANG2("Balinese", ban);
        LANG4("Basque", baq, eus, eu);
        LANG2("Basa", bas);
        LANG2("Baltic languages", bat);
        LANG2("Bedawiyet", bej);
        LANG2("Beja", bej);
        LANG3("Belarusian", bel, be);
        LANG2("Bemba", bem);
        LANG3("Bengali", ben, bn);
        LANG2("Berber languages", ber);
        LANG2("Bhojpuri", bho);
        LANG2("Bihari languages", bih);
        LANG2("Bikol", bik);
        LANG2("Bini", bin);
        LANG3("Bislama", bis, bi);
        LANG2("Siksika", bla);
        LANG2("Bantu languages", bnt);
        LANG3("Bosnian", bos, bs);
        LANG2("Braj", bra);
        LANG3("Breton", bre, br);
        LANG2("Batak languages", btk);
        LANG2("Buriat", bua);
        LANG2("Buginese", bug);
        LANG3("Bulgarian", bul, bg);
        LANG4("Burmese", bur, mya, my);
        LANG2("Bilin", byn);
        LANG2("Blin", byn);
        LANG2("Caddo", cad);
        LANG2("Central American Indian languages", cai);
        LANG2("Galibi Carib", car);
        LANG3("Catalan", cat, ca);
        LANG2("Caucasian languages", cau);
        LANG2("Cebuano", ceb);
        LANG2("Celtic languages", cel);
        LANG3("Chamorro", cha, ch);
        LANG2("Chibcha", chb);
        LANG3("Chechen", che, ce);
        LANG2("Chagatai", chg);
        LANG4("Chinese", chi, zho, zh);
        LANG2("Chuukese", chk);
        LANG2("Mari", chm);
        LANG2("Chinook jargon", chn);
        LANG2("Choctaw", cho);
        LANG2("Chipewyan", chp);
        LANG2("Dene Suline", chp);
        LANG2("Cherokee", chr);
        LANG3("Church Slavic", chu, cu);
        LANG3("Chuvash", chv, cv);
        LANG2("Cheyenne", chy);
        LANG2("Chamic languages", cmc);
        LANG2("Montenegrin", cnr);
        LANG2("Coptic", cop);
        LANG3("Cornish", cor, kw);
        LANG3("Corsican", cos, co);
        LANG2("Creoles and pidgins, English based", cpe);
        LANG2("Creoles and pidgins, French-based", cpf);
        LANG2("Creoles and pidgins, Portuguese-based", cpp);
        LANG3("Cree", cre, cr);
        LANG2("Crimean Tatar", crh);
        LANG2("Crimean Turkish", crh);
        LANG2("Creoles and pidgins", crp);
        LANG2("Kashubian", csb);
        LANG2("Cushitic languages", cus);
        LANG4("Czech", cze, ces, cs);
        LANG2("Dakota", dak);
        LANG3("Danish", dan, da);
        LANG2("Dargwa", dar);
        LANG2("Land Dayak languages", day);
        LANG2("Delaware", del);
        LANG2("Slave (Athapascan)", den);
        LANG2("Tlicho", dgr);
        LANG2("Dinka", din);
        LANG3("Maldivian", div, dv);
        LANG2("Dogri", doi);
        LANG2("Dravidian languages", dra);
        LANG2("Lower Sorbian", dsb);
        LANG2("Duala", dua);
        LANG2("Dutch, Middle (ca.1050-1350)", dum);
        LANG4("Dutch", dut, nld, nl);
        LANG2("Dyula", dyu);
        LANG3("Dzongkha", dzo, dz);
        LANG2("Efik", efi);
        LANG2("Egyptian (Ancient)", egy);
        LANG2("Ekajuk", eka);
        LANG2("Elamite", elx);
        LANG3("English", eng, en);
        LANG2("English, Middle (1100-1500)", enm);
        LANG3("Esperanto", epo, eo);
        LANG3("Estonian", est, et);
        LANG3("Ewe", ewe, ee);
        LANG2("Ewondo", ewo);
        LANG2("Fang", fan);
        LANG3("Faroese", fao, fo);
        LANG2("Fanti", fat);
        LANG3("Fijian", fij, fj);
        LANG2("Filipino", fil);
        LANG3("Finnish", fin, fi);
        LANG2("Finno-Ugrian languages", fiu);
        LANG2("Fon", fon);
        LANG4("French", fre, fra, fr);
        LANG2("French, Middle (ca.1400-1600)", frm);
        LANG2("French, Old (842-ca.1400)", fro);
        LANG2("Northern Frisian", frr);
        LANG2("Eastern Frisian", frs);
        LANG3("Western Frisian", fry, fy);
        LANG3("Fulah", ful, ff);
        LANG2("Friulian", fur);
        LANG2("Ga", gaa);
        LANG2("Gayo", gay);
        LANG2("Gbaya", gba);
        LANG2("Germanic languages", gem);
        LANG4("Georgian", geo, kat, ka);
        LANG4("German", ger, deu, de);
        LANG2("Geez", gez);
        LANG2("Gilbertese", gil);
        LANG3("Gaelic", gla, gd);
        LANG3("Irish", gle, ga);
        LANG3("Galician", glg, gl);
        LANG3("Manx", glv, gv);
        LANG2("German, Middle High (ca.1050-1500)", gmh);
        LANG2("German, Old High (ca.750-1050)", goh);
        LANG2("Gondi", gon);
        LANG2("Gorontalo", gor);
        LANG2("Gothic", got);
        LANG2("Grebo", grb);
        LANG2("Greek, Ancient (to 1453)", grc);
        LANG4("Greek, Modern (1453-)", gre, ell, el);
        LANG3("Guarani", grn, gn);
        LANG2("Swiss German", gsw);
        LANG3("Gujarati", guj, gu);
        LANG2("Gwich'in", gwi);
        LANG2("Haida", hai);
        LANG3("Haitian", hat, ht);
        LANG3("Hausa", hau, ha);
        LANG2("Hawaiian", haw);
        LANG3("Hebrew", heb, he);
        LANG3("Herero", her, hz);
        LANG2("Hiligaynon", hil);
        LANG2("Himachali languages", him);
        LANG2("Western Pahari languages", him);
        LANG3("Hindi", hin, hi);
        LANG2("Hittite", hit);
        LANG2("Hmong", hmn);
        LANG3("Hiri Motu", hmo, ho);
        LANG3("Croatian", hrv, hr);
        LANG2("Upper Sorbian", hsb);
        LANG3("Hungarian", hun, hu);
        LANG2("Hupa", hup);
        LANG2("Iban", iba);
        LANG3("Igbo", ibo, ig);
        LANG4("Icelandic", ice, isl, is);
        LANG3("Ido", ido, io);
        LANG3("Sichuan Yi", iii, ii);
        LANG2("Ijo languages", ijo);
        LANG3("Inuktitut", iku, iu);
        LANG3("Interlingue", ile, ie);
        LANG2("Iloko", ilo);
        LANG3("Interlingua (International Auxiliary Language Association)", ina, ia);
        LANG2("Indic languages", inc);
        LANG3("Indonesian", ind, id);
        LANG2("Indo-European languages", ine);
        LANG2("Ingush", inh);
        LANG3("Inupiaq", ipk, ik);
        LANG2("Iranian languages", ira);
        LANG2("Iroquoian languages", iro);
        LANG3("Italian", ita, it);
        LANG3("Javanese", jav, jv);
        LANG2("Lojban", jbo);
        LANG3("Japanese", jpn, ja);
        LANG2("Judeo-Persian", jpr);
        LANG2("Judeo-Arabic", jrb);
        LANG2("Kara-Kalpak", kaa);
        LANG2("Kabyle", kab);
        LANG2("Kachin", kac);
        LANG3("Greenlandic", kal, kl);
        LANG2("Kamba", kam);
        LANG3("Kannada", kan, kn);
        LANG2("Karen languages", kar);
        LANG3("Kashmiri", kas, ks);
        LANG3("Kanuri", kau, kr);
        LANG2("Kawi", kaw);
        LANG3("Kazakh", kaz, kk);
        LANG2("Kabardian", kbd);
        LANG2("Khasi", kha);
        LANG2("Khoisan languages", khi);
        LANG3("Central Khmer", khm, km);
        LANG2("Khotanese", kho);
        LANG3("Kikuyu", kik, ki);
        LANG3("Kinyarwanda", kin, rw);
        LANG3("Kirghiz", kir, ky);
        LANG2("Kimbundu", kmb);
        LANG2("Konkani", kok);
        LANG3("Komi", kom, kv);
        LANG3("Kongo", kon, kg);
        LANG3("Korean", kor, ko);
        LANG2("Kosraean", kos);
        LANG2("Kpelle", kpe);
        LANG2("Karachay-Balkar", krc);
        LANG2("Karelian", krl);
        LANG2("Kru languages", kro);
        LANG2("Kurukh", kru);
        LANG3("Kuanyama", kua, kj);
        LANG2("Kumyk", kum);
        LANG3("Kurdish", kur, ku);
        LANG2("Kutenai", kut);
        LANG2("Ladino", lad);
        LANG2("Lahnda", lah);
        LANG2("Lamba", lam);
        LANG3("Lao", lao, lo);
        LANG3("Latin", lat, la);
        LANG3("Latvian", lav, lv);
        LANG2("Lezghian", lez);
        LANG3("Limburgan", lim, li);
        LANG3("Lingala", lin, ln);
        LANG3("Lithuanian", lit, lt);
        LANG2("Mongo", lol);
        LANG2("Lozi", loz);
        LANG3("Luxembourgish", ltz, lb);
        LANG2("Luba-Lulua", lua);
        LANG3("Luba-Katanga", lub, lu);
        LANG3("Ganda", lug, lg);
        LANG2("Luiseno", lui);
        LANG2("Lunda", lun);
        LANG2("Luo (Kenya and Tanzania)", luo);
        LANG2("Lushai", lus);
        LANG4("Macedonian", mac, mkd, mk);
        LANG2("Madurese", mad);
        LANG2("Magahi", mag);
        LANG3("Marshallese", mah, mh);
        LANG2("Maithili", mai);
        LANG2("Makasar", mak);
        LANG3("Malayalam", mal, ml);
        LANG2("Mandingo", man);
        LANG4("Maori", mao, mri, mi);
        LANG2("Austronesian languages", map);
        LANG3("Marathi", mar, mr);
        LANG2("Masai", mas);
        LANG4("Malay", may, msa, ms);
        LANG2("Moksha", mdf);
        LANG2("Mandar", mdr);
        LANG2("Mende", men);
        LANG2("Irish, Middle (900-1200)", mga);
        LANG2("Mi'kmaq", mic);
        LANG2("Minangkabau", min);
        LANG2("Uncoded languages", mis);
        LANG2("Mon-Khmer languages", mkh);
        LANG3("Malagasy", mlg, mg);
        LANG3("Maltese", mlt, mt);
        LANG2("Manchu", mnc);
        LANG2("Manipuri", mni);
        LANG2("Manobo languages", mno);
        LANG2("Mohawk", moh);
        LANG3("Mongolian", mon, mn);
        LANG2("Mossi", mos);
        LANG2("Multiple languages", mul);
        LANG2("Munda languages", mun);
        LANG2("Creek", mus);
        LANG2("Mirandese", mwl);
        LANG2("Marwari", mwr);
        LANG2("Mayan languages", myn);
        LANG2("Erzya", myv);
        LANG2("Nahuatl languages", nah);
        LANG2("North American Indian languages", nai);
        LANG2("Neapolitan", nap);
        LANG3("Nauru", nau, na);
        LANG3("Navajo", nav, nv);
        LANG3("Ndebele, South", nbl, nr);
        LANG3("Ndebele, North", nde, nd);
        LANG3("Ndonga", ndo, ng);
        LANG2("German, Low", nds);
        LANG3("Nepali", nep, ne);
        LANG2("Nepal Bhasa", new);
        LANG2("Nias", nia);
        LANG2("Niger-Kordofanian languages", nic);
        LANG2("Niuean", niu);
        LANG3("Norwegian Nynorsk", nno, nn);
        LANG3("Norwegian Bokmål", nob, nb);
        LANG2("Nogai", nog);
        LANG2("Norse, Old", non);
        LANG3("Norwegian", nor, no);
        LANG2("N'Ko", nqo);
        LANG2("Northern Sotho", nso);
        LANG2("Nubian languages", nub);
        LANG2("Classical Nepal Bhasa", nwc);
        LANG2("Classical Newari", nwc);
        LANG3("Nyanja", nya, ny);
        LANG2("Nyamwezi", nym);
        LANG2("Nyankole", nyn);
        LANG2("Nyoro", nyo);
        LANG2("Nzima", nzi);
        LANG3("Occitan (post 1500)", oci, oc);
        LANG3("Ojibwa", oji, oj);
        LANG3("Oriya", ori, or);
        LANG3("Oromo", orm, om);
        LANG2("Osage", osa);
        LANG3("Ossetian", oss, os);
        LANG2("Turkish, Ottoman (1500-1928)", ota);
        LANG2("Otomian languages", oto);
        LANG2("Papuan languages", paa);
        LANG2("Pangasinan", pag);
        LANG2("Pahlavi", pal);
        LANG2("Pampanga", pam);
        LANG3("Punjabi", pan, pa);
        LANG2("Papiamento", pap);
        LANG2("Palauan", pau);
        LANG2("Persian, Old (ca.600-400 B.C.)", peo);
        LANG4("Persian", per, fas, fa);
        LANG2("Philippine languages", phi);
        LANG2("Phoenician", phn);
        LANG3("Pali", pli, pi);
        LANG3("Polish", pol, pl);
        LANG2("Pohnpeian", pon);
        LANG3("Portuguese", por, pt);
        LANG2("Prakrit languages", pra);
        LANG2("Provençal, Old (to 1500)", pro);
        LANG3("Pashto", pus, ps);
        LANG3("Quechua", que, qu);
        LANG2("Rajasthani", raj);
        LANG2("Rapanui", rap);
        LANG2("Rarotongan", rar);
        LANG2("Romance languages", roa);
        LANG3("Romansh", roh, rm);
        LANG2("Romany", rom);
        LANG4("Romanian", rum, ron, ro);
        LANG3("Rundi", run, rn);
        LANG2("Aromanian", rup);
        LANG2("Macedo-Romanian", rup);
        LANG3("Russian", rus, ru);
        LANG2("Sandawe", sad);
        LANG3("Sango", sag, sg);
        LANG2("Yakut", sah);
        LANG2("South American Indian languages", sai);
        LANG2("Salishan languages", sal);
        LANG2("Samaritan Aramaic", sam);
        LANG3("Sanskrit", san, sa);
        LANG2("Sasak", sas);
        LANG2("Santali", sat);
        LANG2("Sicilian", scn);
        LANG2("Scots", sco);
        LANG2("Selkup", sel);
        LANG2("Semitic languages", sem);
        LANG2("Irish, Old (to 900)", sga);
        LANG2("Sign Languages", sgn);
        LANG2("Shan", shn);
        LANG2("Sidamo", sid);
        LANG3("Sinhala", sin, si);
        LANG2("Siouan languages", sio);
        LANG2("Sino-Tibetan languages", sit);
        LANG2("Slavic languages", sla);
        LANG4("Slovak", slo, slk, sk);
        LANG3("Slovenian", slv, sl);
        LANG2("Southern Sami", sma);
        LANG3("Northern Sami", sme, se);
        LANG2("Sami languages", smi);
        LANG2("Lule Sami", smj);
        LANG2("Inari Sami", smn);
        LANG3("Samoan", smo, sm);
        LANG2("Skolt Sami", sms);
        LANG3("Shona", sna, sn);
        LANG3("Sindhi", snd, sd);
        LANG2("Soninke", snk);
        LANG2("Sogdian", sog);
        LANG3("Somali", som, so);
        LANG2("Songhai languages", son);
        LANG3("Sotho, Southern", sot, st);
        LANG3("Spanish", spa, es);
        LANG3("Sardinian", srd, sc);
        LANG2("Sranan Tongo", srn);
        LANG3("Serbian", srp, sr);
        LANG2("Serer", srr);
        LANG2("Nilo-Saharan languages", ssa);
        LANG3("Swati", ssw, ss);
        LANG2("Sukuma", suk);
        LANG3("Sundanese", sun, su);
        LANG2("Susu", sus);
        LANG2("Sumerian", sux);
        LANG3("Swahili", swa, sw);
        LANG3("Swedish", swe, sv);
        LANG2("Classical Syriac", syc);
        LANG2("Syriac", syr);
        LANG3("Tahitian", tah, ty);
        LANG2("Tai languages", tai);
        LANG3("Tamil", tam, ta);
        LANG3("Tatar", tat, tt);
        LANG3("Telugu", tel, te);
        LANG2("Timne", tem);
        LANG2("Tereno", ter);
        LANG2("Tetum", tet);
        LANG3("Tajik", tgk, tg);
        LANG3("Tagalog", tgl, tl);
        LANG3("Thai", tha, th);
        LANG4("Tibetan", tib, bod, bo);
        LANG2("Tigre", tig);
        LANG3("Tigrinya", tir, ti);
        LANG2("Tiv", tiv);
        LANG2("Tokelau", tkl);
        LANG2("Klingon", tlh);
        LANG2("tlhIngan-Hol", tlh);
        LANG2("Tlingit", tli);
        LANG2("Tamashek", tmh);
        LANG2("Tonga (Nyasa)", tog);
        LANG3("Tonga (Tonga Islands)", ton, to);
        LANG2("Tok Pisin", tpi);
        LANG2("Tsimshian", tsi);
        LANG3("Tswana", tsn, tn);
        LANG3("Tsonga", tso, ts);
        LANG3("Turkmen", tuk, tk);
        LANG2("Tumbuka", tum);
        LANG2("Tupi languages", tup);
        LANG3("Turkish", tur, tr);
        LANG2("Altaic languages", tut);
        LANG2("Tuvalu", tvl);
        LANG3("Twi", twi, tw);
        LANG2("Tuvinian", tyv);
        LANG2("Udmurt", udm);
        LANG2("Ugaritic", uga);
        LANG3("Uighur", uig, ug);
        LANG3("Ukrainian", ukr, uk);
        LANG2("Umbundu", umb);
        LANG2("Undetermined", und);
        LANG3("Urdu", urd, ur);
        LANG3("Uzbek", uzb, uz);
        LANG2("Vai", vai);
        LANG3("Venda", ven, ve);
        LANG3("Vietnamese", vie, vi);
        LANG3("Volapük", vol, vo);
        LANG2("Votic", vot);
        LANG2("Wakashan languages", wak);
        LANG2("Wolaitta", wal);
        LANG2("Waray", war);
        LANG2("Washo", was);
        LANG4("Welsh", wel, cym, cy);
        LANG2("Sorbian languages", wen);
        LANG3("Walloon", wln, wa);
        LANG3("Wolof", wol, wo);
        LANG2("Kalmyk", xal);
        LANG2("Oirat", xal);
        LANG3("Xhosa", xho, xh);
        LANG2("Yao", yao);
        LANG2("Yapese", yap);
        LANG3("Yiddish", yid, yi);
        LANG3("Yoruba", yor, yo);
        LANG2("Yupik languages", ypk);
        LANG2("Zapotec", zap);
        LANG2("Bliss", zbl);
        LANG2("Blissymbols", zbl);
        LANG2("Zenaga", zen);
        LANG2("Standard Moroccan Tamazight", zgh);
        LANG3("Zhuang", zha, za);
        LANG2("Zande languages", znd);
        LANG3("Zulu", zul, zu);
        LANG2("Zuni", zun);
        LANG2("No linguistic content", zxx);
        LANG2("Zazaki", zza);
    }
};

struct Language::Detail {
    QString code1, code2, code2_2;
    QString name;

    void init(Language *that) {
        assert(code1.length() == code1Len || code1.isEmpty());
        assert(code2.length() == code2Len);
        assert(code2_2.length() == code2Len || code2_2.isEmpty());
        codeMap.insert({code2, that});
        if (!code1.isEmpty())
            codeMap.insert({code1, that});
        if (!code2_2.isEmpty())
            codeMap.insert({code2_2, that});
        codeMap.insert({name, that});
    }
};

optional<Language const *> Language::fromCode1(QString const &c) {
    if (c.isEmpty())
        return nullopt;
    assert(c.length() == code1Len);
    LanguageFactory_::instance();
    if (codeMap.contains(c))
        return codeMap.at(c);
    return nullopt;
}

optional<Language const *> Language::fromCode2(QString const &c) {
    if (c.isEmpty())
        return nullopt;
    assert(c.length() == code2Len);
    LanguageFactory_::instance();
    if (codeMap.contains(c)) {
        auto r = codeMap.at(c);
        return r;
    }
    return nullopt;
}

optional<Language const *> Language::fromCode(QString const &c) {
    if (c.isEmpty())
        return nullopt;
    if (c.length() == code1Len)
        return fromCode1(c.toLower());
    if (c.length() == code2Len)
        return fromCode2(c.toLower());
    return nullopt;
}

optional<Language const *> Language::fromName(QString const &n) {
    if (n.isEmpty())
        return nullopt;
    LanguageFactory_::instance();
    if (codeMap.contains(n))
        return codeMap.at(n);
    return nullopt;
}

void Language::forEach(LanguageFunction f) {
    LanguageFactory_::instance();
    for (auto const &l : LanguageFactory_::instance()->languages) {
        f(*l.get());
    }
}

bool Language::hasCode1() const {
    return !d->code1.isEmpty();
}

QString const &Language::code1() const {
    return d->code1;
}
QString const &Language::code2() const {
    return d->code2;
}

QString const &Language::name() const {
    return d->name;
}

bool Language::operator==(Language const &o) const {
    return this->d->code2 == o.d->code2 || this->d->code2 == o.d->code2_2;
}

Language::Language(QString const &code2,
                   QString const &code2_2,
                   QString const &code1,
                   QString const &name)
    : d(GREU_MKDET()) {
    d->code2 = code2;
    d->code2_2 = code2_2;
    d->code1 = code1;
    d->name = name;
    d->init(this);
}

QString const Language::Code2::Arabic{"ara"};
QString const Language::Code2::Catalan{"cat"};
QString const Language::Code2::Chinese{"chi"};
QString const Language::Code2::Czech{"cze"};
QString const Language::Code2::Danish{"dan"};
QString const Language::Code2::Dutch{"dut"};
QString const Language::Code2::English{"eng"};
QString const Language::Code2::Finnish{"fin"};
QString const Language::Code2::French{"fra"};
QString const Language::Code2::German{"deu"};
QString const Language::Code2::GreekModern{"gre"};
QString const Language::Code2::Hungarian{"hun"};
QString const Language::Code2::Italian{"ita"};
QString const Language::Code2::Japanese{"jpn"};
QString const Language::Code2::Norwegian{"nor"};
QString const Language::Code2::Portuguese{"por"};
QString const Language::Code2::Polish{"pol"};
QString const Language::Code2::Russian{"rus"};
QString const Language::Code2::Slovak{"slo"};
QString const Language::Code2::Slovenian{"slv"};
QString const Language::Code2::Spanish{"spa"};
QString const Language::Code2::Swedish{"swe"};
QString const Language::Code2::Turkish{"tur"};

} // namespace greu::core
